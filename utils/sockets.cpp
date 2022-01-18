//
// Created by shenk on 28.12.2021.
//

#include "sockets.h"

QMap<QString, std::tuple<int, int, int>> Socket::protocol_socket_args = {
        {"tcp", {AF_INET, SOCK_STREAM, IPPROTO_TCP}},
        {"sctp", {AF_INET, SOCK_STREAM, IPPROTO_SCTP}},
        {"udp", {AF_INET, SOCK_DGRAM, IPPROTO_UDP}},
        {"udplite", {AF_INET, SOCK_DGRAM, IPPROTO_UDPLITE}},

        {"tcp6", {AF_INET6, SOCK_STREAM, IPPROTO_TCP}},
        {"udp6", {AF_INET6, SOCK_DGRAM, IPPROTO_UDP}},
        {"udplite6", {AF_INET6, SOCK_DGRAM, IPPROTO_UDPLITE}},
#ifdef __linux__
        {"mptcp", {AF_INET, SOCK_STREAM, IPPROTO_MPTCP}},
        {"mptcp6", {AF_INET6, SOCK_STREAM, IPPROTO_MPTCP}},
        {"sctp6", {AF_INET6, SOCK_STREAM, IPPROTO_SCTP}}
#endif
};

Socket::Socket(const QString &protocol): protocol(protocol) {

    auto iter = Socket::protocol_socket_args.find(protocol);

    if (iter == Socket::protocol_socket_args.end()) {
        std::cout << "Unsupported protocol" << std::endl;
    }

    std::tie(this->sock_domain, this->sock_type, this->sock_protocol) = *iter;
    this->sock_descriptor = socket(sock_domain, sock_type, sock_protocol);

    if (this->sock_descriptor < 0) {
        std::cout << "Could not create socket" << std::endl;
    } else {
        this->recv_sock_descriptor = sock_descriptor;
    }

    int val = 1;
    setsockopt(sock_descriptor, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

}

Socket::~Socket() {

    // В случае, когда открывали дополнительный сокет с помощью accept (использовали TCP) - его тоже нужно закрыть
    if (recv_sock_descriptor != sock_descriptor) {
        close(recv_sock_descriptor);
    }
    close(sock_descriptor);

}

int Socket::setOpt(int level, int optname, const void *optval, socklen_t optlen) {
    return setsockopt(this->sock_descriptor, level, optname, optval, optlen);
}

const QString& Socket::getProtocol() {
    return this->protocol;
}

int Socket::listenFor(int conn_num) {

    bool is_listen_type = this->sock_type == SOCK_STREAM || this->sock_type == SOCK_SEQPACKET;
    bool is_accept_type = this->sock_type == SOCK_STREAM;

    if (is_listen_type) {
        int err = listen(this->sock_descriptor, conn_num);
        if (err < 0) return err;
    }

    if (is_accept_type) {
        this->recv_sock_descriptor = accept(this->sock_descriptor, NULL, NULL);
        if (this->recv_sock_descriptor < 0) return this->recv_sock_descriptor;
    }

    return 0;
}

int Socket::receiveMsg(msghdr &msg, int flags) {

    int res = recvmsg(recv_sock_descriptor, &msg, flags);

    return res;
}

int Socket::connectTo(const QString &ip_addr, unsigned int port) {

    if (sock_domain == AF_INET) {
        sockaddr_in addr {
                .sin_family = AF_INET,
                .sin_port = htons(port)
        };
        inet_aton(ip_addr.toLocal8Bit().data(), &addr.sin_addr);

        return connect(this->sock_descriptor, (sockaddr *)&addr, sizeof(addr));
    } else if (sock_domain == AF_INET6) {

        sockaddr_in6 addr6 {
            .sin6_family = AF_INET6,
            .sin6_port = htons(port)
        };

        if (ip_addr == "127.0.0.1") {
            inet_pton(AF_INET6, "::1", &addr6.sin6_addr);
        } else {
            inet_pton(AF_INET6, ip_addr.toLocal8Bit().data(), &addr6.sin6_addr);
        }
        return connect(this->sock_descriptor, (sockaddr *)&addr6, sizeof(addr6));
    }

    return 0;
}

int Socket::sendData(const void *data, size_t data_size) {
    return send(this->sock_descriptor, data, data_size, 0);
}

int Socket::bindToAny(unsigned int port) {
    if (sock_type != SOCK_RAW) {

        if (sock_domain == AF_INET) {

            sockaddr_in addr {
                    .sin_family = AF_INET,
                    .sin_port = htons(port),
                    .sin_addr = {
                            .s_addr = INADDR_ANY
                    }
            };
            return bind(this->sock_descriptor, (sockaddr *)&addr, sizeof(addr));

        } else if (sock_domain == AF_INET6) {
            sockaddr_in6 addr6 {
                .sin6_family = AF_INET6,
                .sin6_port = htons(port),
                .sin6_addr = IN6ADDR_ANY_INIT
            };

            return bind(this->sock_descriptor, (sockaddr *)&addr6, sizeof(addr6));
        }
    }

    return 0;
}

