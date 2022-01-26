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

    TimestampsReceiverThread(Socket *_sock, RecvProcessFunc _func, quint64 _packets_count):
    sock(_sock), func(_func), packets_count(_packets_count) {}

    ~TimestampsReceiverThread() {
        delete sock;
        delete receiver;
    }

protected:

    Socket *sock;
    RecvProcessFunc func;

    TimestampsReceiver *receiver;
    quint64 packets_count;

    void run() override {

        if (sock->listenFor(1) < 0) {
            return;
        }

        receiver = new TimestampsReceiver(*sock, func);

        for (int i=0; i<packets_count; i++) {
            emit packetReceived(receiver->recvOne());
        }
    }

signals:

    void packetReceived(const ReceiveTimestamp time_info);

};


#endif //LFNP_TIMESTAMPSRECEIVERTHREAD_H
