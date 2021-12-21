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
    std::vector<SocketInfo> socket_info_list;

    auto sysctl_name = FreeBSDDataSource::protocol_sockets_sysctl_names[protocol];
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

        socket_info_list.emplace_back(
                std::string(loc_addr), std::string(for_addr),
                ntohs(inp->inp_lport), ntohs(inp->inp_fport),
                so->so_rcv.sb_cc, so->so_snd.sb_cc
        );

        std::cout << loc_addr << ":" << ntohs(inp->inp_lport) << " -> " << for_addr << ":" << ntohs(inp->inp_fport);
        std::cout << " " << so->so_rcv.sb_cc << " " << so->so_snd.sb_cc << std::endl;
    }
    return socket_info_list;
}
