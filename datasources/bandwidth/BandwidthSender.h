//
// Created by shenk on 27.01.2022.
//

#ifndef LFNP_BANDWIDTHSENDER_H
#define LFNP_BANDWIDTHSENDER_H

#include <QRunnable>
#include <QThread>
#include <QDebug>
#include "../../utils/sockets.h"

//TODO: Графический интерфейс
class BandwidthSender: public QThread {

private:

    Socket *sock;
    QByteArray data;
    int file_descriptor;
    quint64 data_size;
    bool zero_copy;

    quint64 packets_count;
    quint64 bytes_sent;

    iovec iov;
    msghdr msg;

public:

    BandwidthSender(Socket *_sock, std::unique_ptr<QFile> &_file, quint64 _ds, bool _zc, int cpu_index=-1):
    sock(_sock), file_descriptor(_file->handle()), zero_copy(_zc), data_size(_ds), packets_count(0), bytes_sent(0) {

        iov = {
                .iov_base = &data,
                .iov_len = data_size
        };

        msg = {
                .msg_name = nullptr,
                .msg_namelen = 0,
                .msg_iov = &iov,
                .msg_iovlen = 1,
                .msg_control = nullptr,
                .msg_controllen = 0
        };

        if (!zero_copy) {
            data = _file->read(data_size);
        }

        if (cpu_index >= 0) {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(cpu_index, &cpuset);
            pthread_t current_thread = pthread_self();
            pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
        }

    }

    ~BandwidthSender() {
        delete sock;
    }

    void run() override {

        int err;

        while (!QThread::currentThread()->isInterruptionRequested()) {
            if (zero_copy) {
                err = sock->sendFile(file_descriptor, data_size);
            } else {
                err = sock->sendData(data, data_size);
//                err = sock->sendMsg(msg);
//                err = write(sock->sock_descriptor, data, data_size);
            }

            packets_count++;
            bytes_sent += err;
        }
    }

    quint64 getPacketsCount() { return packets_count; }
    quint64 getBytesSent() { return bytes_sent; }
    void resetCount() {
        packets_count = 0;
        bytes_sent = 0;
    }

};

#endif //LFNP_BANDWIDTHSENDER_H
