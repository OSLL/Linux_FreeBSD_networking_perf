//
// Created by shenk on 17.01.2022.
//

#include "BaseDataSource.h"
#include "timestamping/TimestampsSender.h"
#include "timestamping/TimestampsReceiver.h"
#include "bandwidth/BandwidthSender.h"
#include "bandwidth/BandwidthReceiver.h"
#include <QThreadPool>

using namespace std::placeholders;

std::optional<QVector<ReceiveTimestamp>>
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

std::optional<QVector<SendTimestamp>>
BaseDataSource::sendTimestamps(const QString &protocol, const QString &addr, unsigned int port,
                               unsigned int packets_count, const QString &measure_type, unsigned int delay,
                               const QString &data_filename, quint64 data_size, bool zero_copy) {

    QFile file(data_filename);

    if (!file.open(QIODevice::ReadOnly)) {
        std::cout << "Can't open " << data_filename.toStdString() << std::endl;
        return std::nullopt;
    }

    if (!data_size) {
        data_size = file.size();
    }

    Socket sock(protocol);
    auto o_mt = measure_type_enum.fromString(measure_type);
    if (o_mt) {
        this->setSendSockOpt(sock, *o_mt);
    } else {
        std::cout << "Unknown measure-type" << std::endl;
        return std::nullopt;
    }

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

void BaseDataSource::sendBandwidth(const QString &protocol, const QString &addr, unsigned int port, quint64 duration,
                                   const QString &data_filename, quint64 data_size, bool zero_copy) {

    QFile file(data_filename);

    if (!file.open(QIODevice::ReadOnly)) {
        std::cout << "Can't open " << data_filename.toStdString() << std::endl;
        return;
    }

    if (!data_size) {
        data_size = file.size();
    }
    auto data = file.read(data_size);

    Socket sock(protocol);

    if (sock.connectTo(addr, port) < 0) {
        std::cout << "Connect error" << std::endl;
        return;
    }

    sock.sendData(&data_size);

    QVector<BandwidthSender*> senders;
    for (int i=0; i<4; i++) {
        auto *connection_sock = new Socket(protocol);

        if (connection_sock->connectTo(addr, port) < 0) {
            std::cout << "Connect error" << std::endl;
            return;
        }

        auto sender = new BandwidthSender(connection_sock, data, file.handle(), data_size, zero_copy);
        senders.push_back(sender);
        sender->start();
    }

    QThread::sleep(duration);
    for (auto sender: senders) {
        sender->requestInterruption();
    }
    for (auto sender: senders) {
        sender->wait();
    }

    quint64 packets_count = 0;
    quint64 bytes_sent = 0;

    for (auto sender: senders) {
        packets_count += sender->getPacketsCount();
        bytes_sent += sender->getBytesSent();
    }

    std::cout << "Packets count: " << packets_count << std::endl;
    std::cout << "GBytes: " << (double)bytes_sent/1024/1024/1024 << std::endl;
    std::cout << "GBits/s: " << (double)bytes_sent/duration/1024/1024/128 << std::endl;
}

void BaseDataSource::recvBandwidth(const QString &protocol, unsigned int port) {

    Socket sock(protocol);
    this->setRecvSockOpt(sock);

    if (sock.bindToAny(port) < 0) {
        std::cout << "Bind failed" << std::endl;
        return;
    }

    if (sock.listenFor(8) < 0) {
        std::cout << "Listen failed" << std::endl;
        return;
    }

    quint64 data_size = 0;
    sock.receiveData(&data_size);
    QVector<BandwidthReceiver*> receivers;
    for (int i=0; i<4; i++) {
        auto accept_sock = sock.acceptConnection();
        auto receiver = new BandwidthReceiver(accept_sock, data_size);
        receivers.push_back(receiver);
        receiver->start();
    }

    int duration = 11;
    QThread::sleep(duration);
    for (auto receiver: receivers) {
        receiver->requestInterruption();
    }
    for (auto receiver: receivers) {
        receiver->wait();
    }

    quint64 packets_count = 0;
    quint64 bytes_sent = 0;

    for (auto receiver: receivers) {
        packets_count += receiver->getPacketsCount();
        bytes_sent += receiver->getBytesSent();
    }

    std::cout << "Packets count: " << packets_count << std::endl;
    std::cout << "GBytes: " << (double)bytes_sent/1024/1024/1024 << std::endl;
    std::cout << "GBits/s: " << (double)bytes_sent/duration/1024/1024/128 << std::endl;

}
