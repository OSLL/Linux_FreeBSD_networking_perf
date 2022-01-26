//
// Created by shenk on 26.01.2022.
//

#ifndef LFNP_TIMESTAMPSSENDERTHREAD_H
#define LFNP_TIMESTAMPSSENDERTHREAD_H

#include <QThread>
#include "../../utils/sockets.h"
#include "../../datasources/timestamping/TimestampsSender.h"

class TimestampsSenderThread: public QThread {

    Q_OBJECT

public:

    TimestampsSenderThread(Socket *_sock, QFile *_file, quint64 _data_size, bool _zc, SendProcessFunc _func,
                           quint64 _packets_count):
            sock(_sock), file(_file), data_size(_data_size), zero_copy(_zc), func(_func), packets_count(_packets_count)
            {}

    ~TimestampsSenderThread() {
        delete sock;
        delete file;
        delete receiver;
    }

protected:

    Socket *sock;
    QFile *file;
    quint64 data_size;
    bool zero_copy;
    SendProcessFunc func;

    TimestampsSender *receiver;
    quint64 packets_count;

    void run() override {

        receiver = new TimestampsSender(*sock, *file, data_size, zero_copy, func);

        for (int i=0; i<packets_count; i++) {
            receiver->sendOne();
            emit packetSent(receiver->getInfo());
        }
    }

    signals:

            void packetSent(const InSystemTimeInfo time_info);

};

#endif //LFNP_TIMESTAMPSSENDERTHREAD_H
