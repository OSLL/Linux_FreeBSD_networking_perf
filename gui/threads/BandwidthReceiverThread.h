//
// Created by shenk on 06.02.2022.
//

#ifndef LFNP_BANDWIDTHRECEIVERTHREAD_H
#define LFNP_BANDWIDTHRECEIVERTHREAD_H

#include <QThread>
#include "../../utils/sockets.h"
#include "../../datasources/bandwidth/BandwidthReceiver.h"
#include "../../types/BandwidthResult.h"

class BandwidthReceiverThread: public QThread{

    Q_OBJECT

public:

    BandwidthReceiverThread(std::unique_ptr<Socket> _sock, int _threads_count, bool _cpu_affinity):
    sock(std::move(_sock)), threads_count(_threads_count), cpu_affinity(_cpu_affinity), duration(0), data_size(0) {}

    ~BandwidthReceiverThread() {
        receivers.clear();
    }

protected:
    void run() override {

        if (sock->listenFor(threads_count + 1) < 0) {
            std::cout << "Listen failed" << std::endl;
            return;
        }

        sock->receiveData(&duration);
        sock->receiveData(&data_size);
        emit durationReceived(duration);

        for (int i=0; i < threads_count; i++) {
            auto accept_sock = sock->acceptConnection();

            int cpu_index = cpu_affinity ? i : -1;
            auto receiver = new BandwidthReceiver(accept_sock, data_size, cpu_index);
            receivers.push_back(receiver);
        }

        for (auto &receiver: receivers) {
            receiver->start();
        }

        for (int i=0; i<=duration && !QThread::isInterruptionRequested(); i++) {
            QThread::sleep(1);
            BandwidthResult result;
            result.duration = 1;

            for (auto &receiver: receivers) {
                result.packets_count += receiver->getPacketsCount();
                result.bytes_count += receiver->getBytesSent();
                receiver->resetCount();
            }

            emit bandwidth(result);
        }

        for (auto receiver: receivers) {
            receiver->requestInterruption();
        }

        for (auto receiver: receivers) {
            receiver->wait();
        }
    }

protected:

    std::unique_ptr<Socket> sock;
    QVector<BandwidthReceiver*> receivers;
    quint64 duration;
    quint64 data_size;

    int threads_count;
    bool cpu_affinity;

signals:
    void bandwidth(BandwidthResult result);
    void durationReceived(quint64 duration);
};

#endif //LFNP_BANDWIDTHRECEIVERTHREAD_H
