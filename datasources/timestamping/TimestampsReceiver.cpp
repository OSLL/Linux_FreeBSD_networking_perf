//
// Created by shenk on 25.01.2022.
//

#include "TimestampsReceiver.h"

TimestampsReceiver::TimestampsReceiver(Socket &sock, const QString &protocol, BaseDataSource *ds):
        control_sock(sock), receive_sock(protocol), data_source(ds) {

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

    control_sock.receiveData(&data_size);
    data_source->setRecvSockOpt(receive_sock);
    receive_sock.bindToAny(0);

    auto receive_port = receive_sock.getPort();

    control_sock.sendData(&receive_port);
    receive_sock.listenFor(1);

    iov.iov_base = new char[data_size];
    iov.iov_len = data_size;

}

ReceiveTimestamp TimestampsReceiver::recvOne() {

    bool is_next;
    control_sock.receiveData(&is_next);

    ReceiveTimestamp timestamp;
    auto o_in_call_time = receive_sock.receiveMsgTS(msg, MSG_WAITALL);
    control_sock.receiveData(&timestamp.before_send);

    if (o_in_call_time) {

        timestamp.before_recv = o_in_call_time->from;
        timestamp.after_recv = o_in_call_time->to;
    }

    data_source->processRecvTimestamp(msg, timestamp, o_in_call_time->to, control_sock.getProtocol());
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
