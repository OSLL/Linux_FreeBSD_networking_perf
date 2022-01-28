//
// Created by shenk on 25.01.2022.
//

#ifndef LFNP_TIMESTAMPSSENDER_H
#define LFNP_TIMESTAMPSSENDER_H


#include <QString>
#include <memory>
#include "../../utils/sockets.h"
#include "../../types/InSystemTimeInfo.h"

typedef std::function<void(Socket &sock, SendTimestamp &res, TimeRange &timestamps)> SendProcessFunc;

class TimestampsSender {

protected:

    Socket &sock;
    std::unique_ptr<QFile> file;
    QByteArray data;
    quint64 data_size;
    bool zero_copy;

    QVector<SendTimestamp> time_info;
    SendProcessFunc &send_process_func;

public:

    TimestampsSender(Socket &_sock, std::unique_ptr<QFile> &_file, quint64 _data_size, bool _zero_copy,
                     SendProcessFunc &_func);

    std::optional<SendTimestamp> sendOne(void);
    const QVector<SendTimestamp>& getInfo();
};



#endif //LFNP_TIMESTAMPSSENDER_H
