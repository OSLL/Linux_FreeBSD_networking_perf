//
// Created by shenk on 17.01.2022.
//

#include "BaseDataSource.h"

std::optional<InSystemTimeInfo>
BaseDataSource::recvTimestamp(const QString &protocol, unsigned int port, unsigned int packets_count) {

    Socket sock(protocol);
    this->setRecvSockOpt(sock);

    if (sock.bindToAny(port) < 0) {
        std::cout << "Bind failed" << std::endl;
        return std::nullopt;
    }

    if (sock.listenFor(1) < 0) {
        std::cout << "Listen failed" << std::endl;
        return std::nullopt;
    }

    char control[1000];
    timespec send_time {0, 0};

    iovec iov {
            .iov_base = &send_time,
            .iov_len = sizeof(timespec)
    };

    msghdr msg{
            .msg_name = nullptr,
            .msg_namelen = 0,
            .msg_iov = &iov,
            .msg_iovlen = 1,
            .msg_control = &control,
            .msg_controllen = sizeof(control)
    };

    InSystemTimeInfo res;

    timespec before_recv_time {0, 0}, after_recv_time {0, 0};

    for (int i=0; i<packets_count; i++) {

        clock_gettime(CLOCK_REALTIME, &before_recv_time);
        sock.receiveMsg(msg, 0);
        clock_gettime(CLOCK_REALTIME, &after_recv_time);

        this->processRecvTimestamp(msg, res, after_recv_time, packets_count, protocol);

        timespec_avg_add(res.in_call_time, before_recv_time, after_recv_time, packets_count);
        timespec_avg_add(res.total_time, send_time, after_recv_time, packets_count);
    }

    return res;
}

std::optional<InSystemTimeInfo>
BaseDataSource::sendTimestamp(const QString &protocol, const QString &addr, unsigned int port,
                              unsigned int packets_count, const QString &measure_type, unsigned int delay) {

    Socket sock(protocol);
    this->setSendSockOpt(sock, measure_type);

    if (sock.connectTo(addr, port) < 0) {
        std::cout << "Connect error" << std::endl;
        return std::nullopt;
    }

    char control[1000];
    msghdr msg{
            .msg_name = nullptr,
            .msg_namelen = 0,
            .msg_iov = nullptr,
            .msg_iovlen = 0,
            .msg_control = &control,
            .msg_controllen = sizeof(control)
    };

    InSystemTimeInfo res;

    timespec prev {0, 0},
    before_send_time {0, 0},
    after_send_time {0, 0};

    for (int i=0; i<packets_count; i++) {

        QThread::msleep(delay);

        clock_gettime(CLOCK_REALTIME, &before_send_time);
        auto err = sock.sendData(&before_send_time, sizeof(timespec));
        clock_gettime(CLOCK_REALTIME, &after_send_time);

        if (err < 0) {
            std::cout << "Send error" << std::endl;
            return std::nullopt;
        };

        this->processSendTimestamp(sock, msg, res, before_send_time, packets_count, protocol, prev);
        timespec_avg_add(res.in_call_time, before_send_time, after_send_time, packets_count);
    }

    return res;
}
