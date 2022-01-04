//
// Created by shenk on 17.12.2021.
//

#include "FreeBSDDataSource.h"

std::map<std::string, std::string> FreeBSDDataSource::protocol_sockets_sysctl_names = {
        {"tcp", "net.inet.tcp.pcblist"},
        {"udp", "net.inet.tcp.udp"},
        {"raw", "net.inet.raw.pcblist"}
};

int FreeBSDDataSource::getTcpTotalRecv() {
    tcpstat ts;

    size_t size;
    sysctlbyname("net.inet.tcp.stats", &ts, &size, nullptr, 0);

    return ts.tcps_rcvtotal;
}

std::vector<SocketInfo> FreeBSDDataSource::getSockets(std::string protocol) {

    size_t size;
    std::vector<SocketInfo> sockets_info_list;

     auto iter = FreeBSDDataSource::protocol_sockets_sysctl_names.find(protocol);

    if (iter == FreeBSDDataSource::protocol_sockets_sysctl_names.end()) {
        std::cout << "Unsupported protocol" << std::endl;
        return sockets_info_list;
    }
    auto sysctl_name = iter->second;

    sysctlbyname(sysctl_name.c_str(), nullptr, &size, nullptr, 0);

    char *buf = new char[size];
    sysctlbyname(sysctl_name.c_str(), buf, &size, nullptr, 0);

    struct xtcpcb *tp;
    struct xinpcb *inp;
    struct xinpgen *xig, *oxig;
    struct xsocket *so;

    oxig = xig = (struct xinpgen *)buf;

    for (xig = (struct xinpgen *)((char *)xig + xig->xig_len);
        xig->xig_len > sizeof(struct xinpgen);
        xig = (struct xinpgen *)((char *)xig + xig->xig_len)) {

        xinpcb *inp;

        if (protocol == "tcp") {
            tp = (struct xtcpcb *) xig;
            inp = &tp->xt_inp;
        } else {
            inp = (xinpcb *) xig;
        }

        so = &inp->xi_socket;

        char loc_addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &inp->inp_laddr.s_addr, loc_addr, INET_ADDRSTRLEN);

        char for_addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &inp->inp_faddr.s_addr, for_addr, INET_ADDRSTRLEN);

        sockets_info_list.emplace_back(
                std::string(loc_addr), std::string(for_addr),
                ntohs(inp->inp_lport), ntohs(inp->inp_fport),
                so->so_rcv.sb_cc, so->so_snd.sb_cc
        );
    }
    return sockets_info_list;
}

std::optional<InSystemTimeInfo>
FreeBSDDataSource::recvTimestamp(const QString &protocol, unsigned int port, unsigned int packets_count) {

    Socket sock(protocol);

    if (protocol == "udp") {
        int val = 1;
        sock.setOpt(SOL_SOCKET, SO_TIMESTAMP, &val, sizeof(val));
    }

    if (sock.bindTo(INADDR_ANY, port) < 0) {
        std::cout << "Bind failed" << std::endl;
        return std::nullopt;
    }

    if (sock.getType() == SOCK_STREAM && sock.listenFor(1) < 0) {
        std::cout << "Listen failed" << std::endl;
        return std::nullopt;
    }

    char control[1000];
    timespec user_time, send_time;

    iovec iov {
            .iov_base = &send_time,
            .iov_len = sizeof(timespec)
    };

    msghdr msg{
            .msg_name = nullptr,
            .msg_namelen = 0,
            .msg_iov = &iov,
            .msg_iovlen = 1,
            .msg_control = &control,
            .msg_controllen = sizeof(control)
    };

    timespec *tmst;
    InSystemTimeRXInfo res;

    for (int i=0; i<packets_count; i++) {

        sock.receiveMsg(msg, 0);

        clock_gettime(CLOCK_REALTIME, &user_time);

        if (protocol == "udp") {
            for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
                if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMP) {
                    tmst = (timespec *) CMSG_DATA(cmsg);

                    // Во FreeBSD здесь хранятся микросекунды, но clock_gettime возвращает наносекунды.
                    tmst->tv_nsec *= 1000;
                    timespec_avg_add(res.software_time, *tmst, user_time, packets_count);
                }
            }
        }

        timespec_avg_add(res.total_time, send_time, user_time, packets_count);
    }

    return res;
}

std::optional<InSystemTimeInfo>
FreeBSDDataSource::sendTimestamp(
        const QString &protocol,
        const QString &ip_addr,
        unsigned int port,
        unsigned int packets_count,
        const QString& measure_type) {

    Socket sock(protocol);

    if (sock.connectTo(ip_addr, port) < 0) {
        std::cout << "Connect error" << std::endl;
        return std::nullopt;
    }

    for (int i=0; i<packets_count; i++) {

        timespec user_time = {0, 0};
        clock_gettime(CLOCK_REALTIME, &user_time);

        if (sock.sendData(&user_time, sizeof(timespec)) < 0) {
            std::cout << "Send error" << std::endl;
            return std::nullopt;
        };
    }

    std::cout << "Data sent successfully" << std::endl;

    return std::nullopt;
}
