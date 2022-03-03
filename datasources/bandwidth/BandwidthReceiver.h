//
// Created by shenk on 27.01.2022.
//

#ifndef LFNP_BANDWIDTHRECEIVER_H
#define LFNP_BANDWIDTHRECEIVER_H

#include <QRunnable>
#include <QThread>
#include <QDebug>
#include "../../utils/sockets.h"

#ifdef __linux__
#include "pthread.h"
#else
#include "pthread_np.h"
#define cpu_set_t cpuset_t
#endif

class BandwidthReceiver: public QThread {

private:

    Socket *sock;
    quint64 data_size;

    quint64 packets_count;
    quint64 bytes_sent;

    iovec iov;
    msghdr msg;

public:

    BandwidthReceiver(Socket *_sock, quint64 _ds, int cpu_index=-1):
    sock(_sock), data_size(_ds), packets_count(0), bytes_sent(0) {

        iov = {
                .iov_base = new char[data_size],
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

        if (cpu_index >= 0) {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(cpu_index, &cpuset);
            pthread_t current_thread = pthread_self();
            pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
        }
    }

    ~BandwidthReceiver() {
        delete sock;
        delete[] (char*)msg.msg_iov->iov_base;
    }

    void run() override {

        int err;

        while (!QThread::currentThread()->isInterruptionRequested()) {
            err = sock->receiveMsg(msg, MSG_WAITALL);

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

#endif //LFNP_BANDWIDTHRECEIVER_H