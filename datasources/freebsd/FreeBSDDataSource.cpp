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

//InSystemTimeInfo FreeBSDDataSource::getInSystemTime() {
//
//    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//
//    if (sock < 0) {
//        std::cout << "Could not create socket" << std::endl;
//    }
//
//    int val = 1;
//    int res = setsockopt(sock, SOL_SOCKET, SO_TIMESTAMP, &val, sizeof(val));
////
////    sockaddr_in addr{};
////
////    addr.sin_family = AF_INET;
////    addr.sin_addr.s_addr = INADDR_ANY;
////    addr.sin_port = 7435;
////
////    if (bind(sock, (sockaddr *)&addr, sizeof(addr)) < 0) {
////        std::cout << "Bind failed" << std::endl;
////    }
//
//    char control[1000];
//
//    msghdr msg{
//            .msg_name = nullptr,
//            .msg_namelen = 0,
//            .msg_iov = nullptr,
//            .msg_iovlen = 0,
//            .msg_control = &control,
//            .msg_controllen = sizeof(control)
//    };
//
//    recvmsg(sock, &msg, 0);
//
//    timespec user_time, diff_time;
//    clock_gettime(CLOCK_REALTIME, &user_time);
//
//    for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
//
//        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMP) {
//
//            auto *kernel_time = (timespec*) CMSG_DATA(cmsg);
//            diff_time = timespecsub(user_time, *kernel_time);
//
//        }
//
//    }
//
//    std::cout << "Seconds: " << diff_time.tv_sec << std::endl << "Nanoseconds: " << diff_time.tv_nsec << std::endl;
//
//
//}

std::optional<InSystemTimeRXInfo>
FreeBSDDataSource::getInSystemTimeRX(const QString &protocol, unsigned int port, unsigned int packets_count) {

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
                    timespec_avg_add(res.rx_software_time, *tmst, user_time, packets_count);
                }
            }
        }

        std::cout << "UT: " << user_time.tv_sec << " " << user_time.tv_nsec << std::endl;
        std::cout << "ST: " << send_time.tv_sec << " " << send_time.tv_nsec << std::endl;
        std::cout << "RX: " << tmst->tv_sec << " " << tmst->tv_nsec << std::endl;

        timespec_avg_add(res.rx_total_time, send_time, user_time, packets_count);
    }

    return res;
}

std::optional<InSystemTimeTXInfo>
FreeBSDDataSource::sendTimestamp(const QString &protocol, const QString &addr, unsigned int port,
                                 unsigned int packets_count, const QString &measure_type) {
    return std::nullopt;
}
