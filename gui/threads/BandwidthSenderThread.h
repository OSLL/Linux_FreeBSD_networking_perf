//
// Created by shenk on 20.02.2022.
//

#ifndef LFNP_BANDWIDTHSENDERTHREAD_H
#define LFNP_BANDWIDTHSENDERTHREAD_H

#include <QtGlobal>
#include <utility>
#include "../../datasources/bandwidth/BandwidthSender.h"
#include "../../types/BandwidthResult.h"

class BandwidthSenderThread: public QThread{

    Q_OBJECT

public:

    BandwidthSenderThread(std::unique_ptr<Socket> _sock, int _threads_count, bool _cpu_affinity, bool _zero_copy,
                          quint64 _duration, quint64 _data_size, QString _addr, quint64 _port, std::unique_ptr<QFile> _file):
            sock(std::move(_sock)), threads_count(_threads_count), cpu_affinity(_cpu_affinity), zero_copy(_zero_copy),
            duration(_duration), data_size(_data_size), addr(std::move(_addr)), port(_port), file(std::move(_file)) {}

protected:
    void run() override {

        sock->sendData(&duration);
        sock->sendData(&data_size);

        for (int i=0; i < threads_count; i++) {
            auto *connection_sock = new Socket(sock->getProtocol());

            if (connection_sock->connectTo(addr, port) < 0) {
                qWarning() << "Connect error";
                return;
            }

            int cpu_index = cpu_affinity ? i : -1;
            auto sender = new BandwidthSender(connection_sock, file, data_size, zero_copy, cpu_index);
            senders.push_back(sender);
        }

        for (auto &sender: senders) {
            sender->start();
        }

        for (int i=0; i<=duration && !QThread::isInterruptionRequested(); i++) {
            QThread::sleep(1);
            BandwidthResult result;
            result.duration = 1;

            for (auto &sender: senders) {
                result.packets_count += sender->getPacketsCount();
                result.bytes_count += sender->getBytesSent();
                sender->resetCount();
            }

            emit bandwidth(result);
        }

        for (auto sender: senders) {
            sender->terminate();
        }
        for (auto sender: senders) {
            sender->wait();
        }
    }

protected:

    std::unique_ptr<Socket> sock;
    QVector<BandwidthSender*> senders;
    quint64 duration;
    quint64 data_size;
    QString addr;
    quint64 port;
    std::unique_ptr<QFile> file;

    int threads_count;
    bool cpu_affinity;
    bool zero_copy;

    signals:
            void bandwidth(BandwidthResult result);
};

#endif //LFNP_BANDWIDTHSENDERTHREAD_H
