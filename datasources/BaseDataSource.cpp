//
// Created by shenk on 17.01.2022.
//

#include "BaseDataSource.h"
#include "timestamping/TimestampsSender.h"
#include "timestamping/TimestampsReceiver.h"

using namespace std::placeholders;

std::optional<InSystemTimeInfo>
BaseDataSource::recvTimestamps(const QString &protocol, unsigned int port, unsigned int packets_count) {

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

    RecvProcessFunc recv_func = std::bind(&BaseDataSource::processRecvTimestamp, this, _1, _2, _3, _4);
    TimestampsReceiver receiver(sock, recv_func);

    for (int i=0; i<packets_count; i++) {
        receiver.recvOne();
    }

    return receiver.getInfo();
}

std::optional<InSystemTimeInfo>
BaseDataSource::sendTimestamps(const QString &protocol, const QString &addr, unsigned int port,
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

    SendProcessFunc send_func = std::bind(&BaseDataSource::processSendTimestamp, this, _1, _2, _3);
    TimestampsSender sender(sock, file, data_size, zero_copy, send_func);

    for (int i=0; i<packets_count; i++) {

        QThread::msleep(delay);
        sender.sendOne();
    }

    return sender.getInfo();
}
