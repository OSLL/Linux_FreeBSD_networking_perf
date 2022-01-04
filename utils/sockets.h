//
// Created by shenk on 28.12.2021.
//

#ifndef LFNP_SOCKETS_H
#define LFNP_SOCKETS_H

#include <QString>
#include <QMap>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <iostream>

#ifdef __linux__
#include <libnet.h>
#else
#include <arpa/inet.h>
#endif

class Socket {

private:
    int sock_descriptor;
    int sock_domain;
    int sock_type;
    int sock_protocol;

    static QMap<QString, std::tuple<int, int, int>> protocol_socket_args;

public:
    explicit Socket(const QString &protocol);
    ~Socket();

    int setOpt(int level, int optname, const void * optval, socklen_t optlen);

    int getDomain();
    int getType();
    int getProtocol();

    int bindTo(in_addr_t ip_addr, unsigned int port);
    int bindTo(const QString &ip_addr, unsigned int port);

    int listenFor(int conn_num);
    int receiveMsg(msghdr &msg, int flags = 0, bool do_accept = true);

    int connectTo(const QString &ip_addr, unsigned int port);

    int sendData(const void *data, size_t data_size);
};

#endif //LFNP_SOCKETS_H
