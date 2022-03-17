//
// Created by shenk on 25.01.2022.
//

#ifndef LFNP_TIMESTAMPSSENDER_H
#define LFNP_TIMESTAMPSSENDER_H


#include <QString>
#include <memory>
#include "../../utils/sockets.h"
#include "../../types/InSystemTimeInfo.h"
#include "../BaseDataSource.h"

class TimestampsSender {

protected:

    Socket &control_sock;
    Socket send_sock;
    std::unique_ptr<QFile> file;
    QByteArray data;
    quint64 data_size;
    bool zero_copy;

    QVector<SendTimestamp> time_info;
    BaseDataSource *data_source;

public:

    TimestampsSender(Socket &_sock, const QString &send_addr, const QString &protocol, std::unique_ptr<QFile> &_file, quint64 _data_size, bool _zero_copy,
                     BaseDataSource *ds, MeasureType measure_type);

    std::optional<SendTimestamp> sendOne();
    const QVector<SendTimestamp>& getInfo();
};



#endif //LFNP_TIMESTAMPSSENDER_H
