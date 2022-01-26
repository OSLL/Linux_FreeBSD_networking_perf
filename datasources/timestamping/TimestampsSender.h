//
// Created by shenk on 25.01.2022.
//

#ifndef LFNP_TIMESTAMPSSENDER_H
#define LFNP_TIMESTAMPSSENDER_H


#include <QString>
#include "../../utils/sockets.h"
#include "../../types/InSystemTimeInfo.h"

typedef std::function<void(Socket &sock, SendTimestamp &res, TimeRange &timestamps)> SendProcessFunc;

class TimestampsSender {

protected:

    Socket &sock;
    const QFile &file;
    QByteArray data;
    quint64 data_size;
    bool zero_copy;

    QVector<SendTimestamp> time_info;
    SendProcessFunc &send_process_func;

public:

    TimestampsSender(Socket &sock, QFile &file, quint64 data_size, bool zero_copy, SendProcessFunc &func);

    std::optional<SendTimestamp> sendOne(void);
    const QVector<SendTimestamp>& getInfo();
};



#endif //LFNP_TIMESTAMPSSENDER_H
