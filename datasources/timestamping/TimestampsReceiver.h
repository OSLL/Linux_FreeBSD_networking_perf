//
// Created by shenk on 25.01.2022.
//

#ifndef LFNP_TIMESTAMPSRECEIVER_H
#define LFNP_TIMESTAMPSRECEIVER_H

#include <QFile>
#include "../../utils/sockets.h"
#include "../../types/InSystemTimeInfo.h"
#include "../BaseDataSource.h"

#define CONTROL_SIZE 1024

typedef std::function<void(msghdr &msg, ReceiveTimestamp &res, timespec &after_recv_time,
                           const QString &protocol)> RecvProcessFunc;

class TimestampsReceiver {

private:

    Socket &control_sock;
    Socket receive_sock;
    quint64 data_size;

    iovec iov;
    msghdr msg;

    QVector<ReceiveTimestamp> time_info;
    BaseDataSource *data_source;

public:

    TimestampsReceiver(Socket &sock, const QString &protocol, BaseDataSource *ds);
    ~TimestampsReceiver();

    ReceiveTimestamp recvOne();
    const QVector<ReceiveTimestamp>& getInfo();
};


#endif //LFNP_TIMESTAMPSRECEIVER_H
