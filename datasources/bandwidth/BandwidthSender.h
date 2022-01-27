//
// Created by shenk on 27.01.2022.
//

#ifndef LFNP_BANDWIDTHSENDER_H
#define LFNP_BANDWIDTHSENDER_H

#include <QRunnable>
#include <QThread>
#include <QDebug>
#include "../../utils/sockets.h"

class BandwidthSender: public QThread {

private:

    Socket sock;
    QByteArray data;
    int file_descriptor;
    quint64 data_size;
    bool zero_copy;

    quint64 packets_count;
    quint64 bytes_sent;

    iovec iov;
    msghdr msg;

public:

    BandwidthSender(const Socket& _sock, QByteArray _data, int _fd, quint64 _ds, bool _zc):
    sock(_sock), data(_data), file_descriptor(_fd), zero_copy(_zc), data_size(_ds), packets_count(0), bytes_sent(0) {

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

    }

    void run() override {

        int err = 0;

        while (!QThread::currentThread()->isInterruptionRequested()) {
            if (zero_copy) {
                err = sock.sendFile(file_descriptor, data_size);
            } else {
//                err = sock.sendData(data, data_size);
                err = sock.sendMsg(msg);
            }

            if (err >= 0) {
                packets_count++;
                bytes_sent += err;
            } else {
                qDebug() << "Error: " << err << strerror(errno);
            }
        }

        qDebug() << packets_count << bytes_sent;
    }

    quint64 getPacketsCount() { return packets_count; }
    quint64 getBytesSent() { return bytes_sent; }

};

#endif //LFNP_BANDWIDTHSENDER_H
