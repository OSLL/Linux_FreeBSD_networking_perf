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

    unsigned int data_size;
    sock.receiveData(&data_size);

    char data[data_size];
    iovec iov {
            .iov_base = data,
            .iov_len = data_size
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

    for (int i=0; i<packets_count; i++) {

        auto o_timestamps = sock.receiveMsg(msg, 0);
        if (!o_timestamps) {
            std::cout << "Error in receive message" << std::endl;
            return std::nullopt;
        }

        this->processRecvTimestamp(msg, res, o_timestamps->after_op_time, packets_count, protocol);

        sock.receiveData(&send_time);

        timespec_avg_add(res.in_call_time, o_timestamps->before_op_time, o_timestamps->after_op_time, packets_count);
        timespec_avg_add(res.total_time, send_time, o_timestamps->after_op_time, packets_count);
    }

    return res;
}

std::optional<InSystemTimeInfo>
BaseDataSource::sendTimestamp(const QString &protocol, const QString &addr, unsigned int port,
                              unsigned int packets_count, const QString &measure_type, unsigned int delay,
                              const QString &data_filename, unsigned int data_size, bool zero_copy) {

    QFile file(data_filename);

    if (!file.open(QIODevice::ReadOnly)) {
        std::cout << "Can't open " << data_filename.toStdString() << std::endl;
        return std::nullopt;
    }

    if (!data_size) {
        data_size = file.size();
    }

    Socket sock(protocol);
    this->setSendSockOpt(sock, measure_type);

    if (sock.connectTo(addr, port) < 0) {
        std::cout << "Connect error" << std::endl;
        return std::nullopt;
    }

    InSystemTimeInfo res;
    timespec prev {0, 0};

    sock.sendData(&data_size);

    for (int i=0; i<packets_count; i++) {

        QThread::msleep(delay);

        std::optional<SocketOpTimestamps> o_timestamps;
        if (zero_copy) {
            o_timestamps = sock.sendFile(file.handle(), data_size);
        } else {
            o_timestamps = sock.sendData(file.read(data_size).data(), data_size);
        }
        if (!o_timestamps) {
            std::cout << "Error in send data" << std::endl;
            return std::nullopt;
        }

        std::cout << "BFT: " << o_timestamps->before_op_time.tv_sec << " " << o_timestamps->before_op_time.tv_nsec << std::endl;
        this->processSendTimestamp(sock, res, o_timestamps->before_op_time, packets_count, protocol, prev);
        std::cout << "AFT: " << o_timestamps->after_op_time.tv_sec << " " << o_timestamps->after_op_time.tv_nsec << std::endl << std::endl;
        sock.sendData(&o_timestamps->before_op_time, sizeof(timespec));

        timespec_avg_add(res.in_call_time, o_timestamps->before_op_time, o_timestamps->after_op_time, packets_count);
    }

    return res;
}
