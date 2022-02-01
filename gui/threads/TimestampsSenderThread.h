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

    TimestampsSenderThread(Socket *_sock, std::unique_ptr<QFile> &_file, quint64 _data_size, bool _zc,
                           SendProcessFunc _func, quint64 _packets_count, quint64 _delay) :
            sock(_sock), file(std::move(_file)), data_size(_data_size), zero_copy(_zc), func(_func),
            packets_count(_packets_count), delay(_delay)
            {}

    ~TimestampsSenderThread() {
        delete sock;
        delete sender;
    }

protected:

    Socket *sock;
    std::unique_ptr<QFile> file;
    quint64 data_size;
    quint64 delay;
    bool zero_copy;
    SendProcessFunc func;

    TimestampsSender *sender;
    quint64 packets_count;

    void run() override {

        sender = new TimestampsSender(*sock, file, data_size, zero_copy, func);

        for (int i=0; i<packets_count && !QThread::isInterruptionRequested(); i++) {
            QThread::msleep(delay);
            emit packetSent(sender->sendOne());
        }
    }

    signals:

            void packetSent(const std::optional<SendTimestamp> o_send_ts);

};

#endif //LFNP_TIMESTAMPSSENDERTHREAD_H
