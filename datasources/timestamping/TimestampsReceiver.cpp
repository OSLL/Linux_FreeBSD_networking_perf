//
// Created by shenk on 25.01.2022.
//

#include "TimestampsReceiver.h"
#include <QDebug>

TimestampsReceiver::TimestampsReceiver(Socket &sock, RecvProcessFunc &func):
sock(sock), recv_process_func(func) {

    // Объявление iov должно быть до receiveData. В противном случае, при нажатии на кнопку "Стоп" может быть вызван
    // деструктор, но iov еще не объявлен, а значит происходит освобождение некорректного адреса - segfault

    iov = {
            .iov_base = nullptr,
            .iov_len = 0
    };

    msg = {
            .msg_name = nullptr,
            .msg_namelen = 0,
            .msg_iov = &iov,
            .msg_iovlen = 1,
            .msg_control = new char[CONTROL_SIZE],
            .msg_controllen = CONTROL_SIZE
    };

    sock.receiveData(&data_size);

    iov.iov_base = new char[data_size];
    iov.iov_len = data_size;

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
