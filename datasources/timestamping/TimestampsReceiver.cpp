//
// Created by shenk on 25.01.2022.
//

#include "TimestampsReceiver.h"
#include <QDebug>

TimestampsReceiver::TimestampsReceiver(Socket &sock, RecvProcessFunc &func):
sock(sock), recv_process_func(func) {

    sock.receiveData(&data_size);

    iov = {
            .iov_base = new char[data_size],
            .iov_len = data_size
    };

    msg = {
            .msg_name = nullptr,
            .msg_namelen = 0,
            .msg_iov = &iov,
            .msg_iovlen = 1,
            .msg_control = new char[CONTROL_SIZE],
            .msg_controllen = CONTROL_SIZE
    };
}

ReceiveTimestamp TimestampsReceiver::recvOne() {

    ReceiveTimestamp timestamp;
    auto o_in_call_time = sock.receiveMsgTS(msg, MSG_WAITALL);
    sock.receiveData(&timestamp.before_send);

    if (o_in_call_time) {

        timestamp.before_recv = o_in_call_time->from;
        timestamp.after_recv = o_in_call_time->to;
    }

    recv_process_func(msg, timestamp, o_in_call_time->to, sock.getProtocol());
    time_info.push_back(timestamp);
    return timestamp;
}

TimestampsReceiver::~TimestampsReceiver() {

    delete[] (char*)iov.iov_base;
    delete[] (char*)msg.msg_control;

}

const QVector<ReceiveTimestamp> &TimestampsReceiver::getInfo() {
    return time_info;
}
