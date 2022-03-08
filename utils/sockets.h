//
// Created by shenk on 28.12.2021.
//

#ifndef LFNP_SOCKETS_H
#define LFNP_SOCKETS_H

#include <QString>
#include <QMap>
#include <QFile>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/uio.h>
#include <arpa/inet.h>

#include <iostream>
#include <optional>

#include "../types/TimeRange.h"

#ifdef __linux__
#include <linux/net_tstamp.h>
#include <sys/sendfile.h>
#else
#include <sys/types.h>
#endif

class Socket {

private:
    int sock_descriptor;
    int sock_domain;
    int sock_type;
    int sock_protocol;

    // Для TCP прием сообщения происходит не с помощью полученного сокета, а с помощью сокета, полученного после
    // вызова accept
    int recv_sock_descriptor;

    const QString addr;
    const QString protocol;

    static QMap<QString, std::tuple<int, int, int>> protocol_socket_args;

public:
    explicit Socket(const QString &protocol);
    Socket(int descriptor, const QString &protocol);
    ~Socket();

    int setOpt(int level, int optname, const void * optval, socklen_t optlen);

    const QString& getProtocol();

    int bindToAny(unsigned int port);
    unsigned int getPort();
    QString getAddr();

    int listenFor(int conn_num);
    Socket *acceptConnection();

    template <typename T>
    int receiveData(T *data, int flags=0) { return recv(recv_sock_descriptor, data, sizeof(T), flags); }
    int receiveData(void *data, size_t data_size, int flags=0) { return recv(recv_sock_descriptor, data, data_size, flags); }
    int receiveMsg(msghdr &msg, int flags = 0) { return recvmsg(recv_sock_descriptor, &msg, flags); }
    std::optional<TimeRange> receiveMsgTS(msghdr &msg, int flags = 0);

    int connectTo(const QString &ip_addr, unsigned int port);

    template <typename T>
    int sendData(T* data) { return send(recv_sock_descriptor, data, sizeof(T), 0); }
    int sendData(const void *data, size_t data_size) { return send(recv_sock_descriptor, data, data_size, 0); }
    int sendMsg(const msghdr &msg, int flags=0) { return sendmsg(recv_sock_descriptor, &msg, flags); }

    int sendFile(int file_descriptor, size_t data_size);
    std::optional<TimeRange> sendDataTS(const void *data, size_t data_size);
    std::optional<TimeRange> sendFileTS(int file_descriptor, size_t data_size);

    static QStringList getSupportedProtocols();
};

#endif //LFNP_SOCKETS_H
