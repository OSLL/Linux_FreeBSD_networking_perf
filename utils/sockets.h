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

#include <iostream>
#include <optional>

#ifdef __linux__
#include <libnet.h>
#include <linux/net_tstamp.h>
#include <sys/sendfile.h>
#include "../types/TimeRange.h"

#else
#include <arpa/inet.h>
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
    const QString &protocol;

    static QMap<QString, std::tuple<int, int, int>> protocol_socket_args;

public:
    explicit Socket(const QString &protocol);
    ~Socket();

    int setOpt(int level, int optname, const void * optval, socklen_t optlen);

    const QString& getProtocol();

    int bindToAny(unsigned int port);

    int listenFor(int conn_num);

    template <typename T>
    int receiveData(T *data) { return recv(recv_sock_descriptor, data, sizeof(T), 0); }
    std::optional<TimeRange> receiveMsg(msghdr &msg, int flags = 0);

    int connectTo(const QString &ip_addr, unsigned int port);

    template <typename T>
    std::optional<TimeRange> sendData(T* data) { return sendData(data, sizeof(T)); }
    std::optional<TimeRange> sendData(const void *data, size_t data_size);
    std::optional<TimeRange> sendFile(int file_descriptor, size_t data_size);

    static QStringList getSupportedProtocols();
};

#endif //LFNP_SOCKETS_H
