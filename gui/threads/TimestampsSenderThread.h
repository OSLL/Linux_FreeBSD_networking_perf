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
        delete sender;
    }

protected:

    Socket *sock;
    QFile *file;
    quint64 data_size;
    bool zero_copy;
    SendProcessFunc func;

    TimestampsSender *sender;
    quint64 packets_count;

    void run() override {

        sender = new TimestampsSender(*sock, *file, data_size, zero_copy, func);

        for (int i=0; i<packets_count; i++) {
            emit packetSent(sender->sendOne());
        }
    }

    signals:

            void packetSent(const std::optional<SendTimestamp> o_send_ts);

};

#endif //LFNP_TIMESTAMPSSENDERTHREAD_H
