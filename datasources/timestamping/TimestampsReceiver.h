//
// Created by shenk on 25.01.2022.
//

#ifndef LFNP_TIMESTAMPSRECEIVER_H
#define LFNP_TIMESTAMPSRECEIVER_H

#include <QFile>
#include "../../utils/sockets.h"
#include "../../types/InSystemTimeInfo.h"

#define CONTROL_SIZE 1024

typedef std::function<void(msghdr &msg, ReceiveTimestamp &res, timespec &after_recv_time,
                           const QString &protocol)> RecvProcessFunc;

class TimestampsReceiver {

private:

    Socket &sock;
    quint64 data_size;

    iovec iov;
    msghdr msg;

    QVector<ReceiveTimestamp> time_info;
    RecvProcessFunc &recv_process_func;

public:

    TimestampsReceiver(Socket &sock, RecvProcessFunc &func);
    ~TimestampsReceiver();

    ReceiveTimestamp recvOne();
    const QVector<ReceiveTimestamp>& getInfo();
};


#endif //LFNP_TIMESTAMPSRECEIVER_H
