//
// Created by shenk on 17.01.2022.
//

#include "BaseDataSource.h"
#include "timestamping/TimestampsSender.h"
#include "timestamping/TimestampsReceiver.h"
#include "bandwidth/BandwidthSender.h"
#include "bandwidth/BandwidthReceiver.h"
#include "../types/BandwidthResult.h"
#include "../utils/default_args.h"
#include <QThreadPool>

using namespace std::placeholders;

std::optional<QVector<ReceiveTimestamp>>
BaseDataSource::recvTimestamps(const QString &protocol, unsigned int port, unsigned int packets_count) {

    Socket sock("tcp");

    if (sock.bindToAny(port) < 0) {
        std::cout << "Bind failed" << std::endl;
        return std::nullopt;
    }

    if (sock.listenFor(1) < 0) {
        std::cout << "Listen failed" << std::endl;
        return std::nullopt;
    }

    TimestampsReceiver receiver(sock, protocol, this);

    for (int i=0; i<packets_count; i++) {
        receiver.recvOne();
    }

    return receiver.getInfo();
}

std::optional<QVector<SendTimestamp>>
BaseDataSource::sendTimestamps(const QString &protocol, const QString &addr, unsigned int port,
                               unsigned int packets_count, const QString &measure_type, unsigned int delay,
                               const QString &data_filename, quint64 data_size, bool zero_copy) {

    auto o_file = get_file(data_filename, data_size ? data_size : DEFAULT_NOT_ZERO_DATASIZE);
    if (!o_file) {
        return std::nullopt;
    }
    auto file = std::move(*o_file);

    if (!data_size) {
        data_size = file->size();
    }

    Socket sock("tcp");

    if (sock.connectTo(addr, port) < 0) {
        std::cout << "Connect error" << std::endl;
        return std::nullopt;
    }

    auto o_mt = measure_type_enum.fromString(measure_type);
    if (!o_mt) {
        std::cout << "Unknown measure-type" << std::endl;
        return std::nullopt;
    }
    auto mt = o_mt.value();

    TimestampsSender sender(sock, addr, protocol, file, data_size, zero_copy, this, mt);

    for (int i=0; i<packets_count; i++) {

        QThread::msleep(delay);
        sender.sendOne();
    }

    return sender.getInfo();
}

std::optional<BandwidthResult> BaseDataSource::sendBandwidth(const QString &protocol, const QString &addr, unsigned int port, quint64 duration,
                                   const QString &data_filename, quint64 data_size, bool zero_copy, quint64 threads_count, bool cpu_affinity) {

    auto o_file = get_file(data_filename, data_size ? data_size : DEFAULT_NOT_ZERO_DATASIZE);
    if (!o_file) {
        return std::nullopt;
    }
    auto file = std::move(*o_file);

    if (!data_size) {
        data_size = file->size();
    }

    Socket sock(protocol);

    if (sock.connectTo(addr, port) < 0) {
        std::cout << "Connect error" << std::endl;
        return std::nullopt;
    }

    sock.sendData(&duration);
    sock.sendData(&data_size);

    QVector<BandwidthSender*> senders;
    for (int i=0; i < threads_count; i++) {
        auto *connection_sock = new Socket(protocol);

        if (connection_sock->connectTo(addr, port) < 0) {
            std::cout << "Connect error" << std::endl;
            return std::nullopt;
        }

        int cpu_index = cpu_affinity ? i : -1;
        auto sender = new BandwidthSender(connection_sock, file, data_size, zero_copy, cpu_index);
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

    BandwidthResult res;
    res.duration = duration;

    for (auto sender: senders) {
        res.packets_count += sender->getPacketsCount();
        res.bytes_count += sender->getBytesSent();
    }

    return res;
}

std::optional<BandwidthResult> BaseDataSource::recvBandwidth(const QString &protocol, unsigned int port,
                                                             quint64 threads_count, bool cpu_affinity) {

    Socket sock(protocol);

    if (sock.bindToAny(port) < 0) {
        std::cout << "Bind failed" << std::endl;
        return std::nullopt;
    }

    // Так как сервер запустит threads_count accept-ов, то нужно слушать threads_count + основной, через который будут
    // получены duration и data_size
    if (sock.listenFor(threads_count + 1) < 0) {
        std::cout << "Listen failed" << std::endl;
        return std::nullopt;
    }

    quint64 duration = 0;
    quint64 data_size = 0;

    sock.receiveData(&duration);
    sock.receiveData(&data_size);

    QVector<BandwidthReceiver*> receivers;
    for (int i=0; i < threads_count; i++) {
        auto accept_sock = sock.acceptConnection();

        int cpu_index = cpu_affinity ? i : -1;
        auto receiver = new BandwidthReceiver(accept_sock, data_size, cpu_index);
        receivers.push_back(receiver);
        receiver->start();
    }

    QThread::sleep(duration);

    for (auto receiver: receivers) {
        receiver->terminate();
    }
    for (auto receiver: receivers) {
        receiver->wait();
    }

    BandwidthResult res;
    res.duration = duration;

    for (auto receiver: receivers) {
        res.packets_count += receiver->getPacketsCount();
        res.bytes_count += receiver->getBytesSent();
    }

    return res;
}

int BaseDataSource::getOneStat(const QString &protocol, const QString &stat_name) {

    auto stats = getProtocolStats(protocol);
    return stats->value(stat_name);

}

std::optional<SocketInfo> BaseDataSource::getOneSocket(const QString &protocol, const QString &local_address, unsigned int local_port,
                                                       const QString &foreign_address, unsigned int foreign_port) {
    auto sockets_list = getSockets(protocol);
    for (const auto &si: sockets_list) {
        if (si.local_address == local_address && si.foreign_address == foreign_address &&
            si.local_port == local_port && si.foreign_port == foreign_port) {

            return si;

        }
    }

    return std::nullopt;
}

std::optional<ProfilerParser> BaseDataSource::getProfilerData() {
    auto collector = this->getProfilerCollector();
    collector->onStart();

    for (int i=0; i<10; i++) {
        QThread::sleep(1);
        collector->onTimer();
    }

    return collector->onEnd();
}
