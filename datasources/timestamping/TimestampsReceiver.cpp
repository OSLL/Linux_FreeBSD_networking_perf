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

void TimestampsReceiver::recvOne() {

    auto o_in_call_time = sock.receiveMsg(msg, MSG_WAITALL);

    timespec send_time = {0, 0};
    sock.receiveData(&send_time);

    if (o_in_call_time) {
        auto total_time = TimeRange(send_time, o_in_call_time->to).getRangeNS();

        time_info.in_call_time.push_back(o_in_call_time->getRangeNS());
        time_info.total_time.push_back(total_time);
    }

    recv_process_func(msg, time_info, o_in_call_time->to, sock.getProtocol());
}

TimestampsReceiver::~TimestampsReceiver() {

    delete[] (char*)iov.iov_base;
    delete[] (char*)msg.msg_control;

}

const InSystemTimeInfo &TimestampsReceiver::getInfo() {
    return time_info;
}
