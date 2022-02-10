//
// Created by shenk on 25.01.2022.
//

#ifndef LFNP_TIMESTAMPSRECEIVERTHREAD_H
#define LFNP_TIMESTAMPSRECEIVERTHREAD_H

#include <QThread>

#include "../../datasources/timestamping/TimestampsReceiver.h"

class TimestampsReceiverThread: public QThread {

    Q_OBJECT

public:

    TimestampsReceiverThread(Socket *_sock, QString _protocol, BaseDataSource *ds, quint64 _packets_count):
    sock(_sock), protocol(std::move(_protocol)), data_source(ds), packets_count(_packets_count), receiver(nullptr) {}

    ~TimestampsReceiverThread() {
        delete sock;
        delete receiver;
    }

protected:

    Socket *sock;
    RecvProcessFunc func;
    const QString protocol;

    TimestampsReceiver *receiver;
    quint64 packets_count;
    BaseDataSource *data_source;

    void run() override {

        if (sock->listenFor(1) < 0) {
            return;
        }

        receiver = new TimestampsReceiver(*sock, protocol, data_source);

        for (int i=0; i<packets_count && !QThread::isInterruptionRequested(); i++) {
            emit packetReceived(receiver->recvOne());
        }
    }

signals:

    void packetReceived(const ReceiveTimestamp time_info);

};


#endif //LFNP_TIMESTAMPSRECEIVERTHREAD_H
