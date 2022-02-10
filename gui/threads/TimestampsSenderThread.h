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

    TimestampsSenderThread(Socket *_sock, QString _protocol, std::unique_ptr<QFile> &_file, quint64 _data_size, bool _zc,
                           BaseDataSource *ds, quint64 _packets_count, quint64 _delay, MeasureType _measure_type) :
            sock(_sock),  protocol(std::move(_protocol)), file(std::move(_file)), data_size(_data_size), zero_copy(_zc), data_source(ds),
            packets_count(_packets_count), delay(_delay), measure_type(_measure_type)
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
    BaseDataSource *data_source;
    const QString protocol;
    MeasureType measure_type;

    TimestampsSender *sender;
    quint64 packets_count;

    void run() override {

        sender = new TimestampsSender(*sock, protocol, file, data_size, zero_copy, data_source, measure_type);

        for (int i=0; i<packets_count && !QThread::isInterruptionRequested(); i++) {
            QThread::msleep(delay);
            emit packetSent(sender->sendOne());
        }
    }

    signals:

            void packetSent(const std::optional<SendTimestamp> o_send_ts);

};

#endif //LFNP_TIMESTAMPSSENDERTHREAD_H
