//
// Created by shenk on 17.12.2021.
//

#include "FreeBSDDataSource.h"

int FreeBSDDataSource::getTcpTotalRecv() {
    tcpstat ts;

    size_t size;
    sysctlbyname("net.inet.tcp.stats", &ts, &size, nullptr, 0);

    return ts.tcps_rcvtotal;
}

int FreeBSDDataSource::getTcpConnList() {

    size_t size;
    sysctlbyname("net.inet.tcp.pcblist", nullptr, &size, nullptr, 0);

    char *buf = new char[size];
    sysctlbyname("net.inet.tcp.pcblist", buf, &size, nullptr, 0);

    struct xtcpcb *tp;
    struct xinpcb *inp;
    struct xinpgen *xig, *oxig;
    struct xsocket *so;

    oxig = xig = (struct xinpgen *)buf;

    for (xig = (struct xinpgen *)((char *)xig + xig->xig_len);
        xig->xig_len > sizeof(struct xinpgen);
        xig = (struct xinpgen *)((char *)xig + xig->xig_len)) {
        tp = (struct xtcpcb *) xig;
        inp = &tp->xt_inp;
        so = &inp->xi_socket;

        char src_addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &inp->inp_laddr.s_addr, src_addr, INET_ADDRSTRLEN);

        char dst_addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &inp->inp_faddr.s_addr, dst_addr, INET_ADDRSTRLEN);

        std::cout << src_addr << ":" << ntohs(inp->inp_lport) << " -> " << dst_addr << ":" << ntohs(inp->inp_fport);
        std::cout << " " << so->so_rcv.sb_cc << " " << so->so_snd.sb_cc << std::endl;
    }
    return 0;
}
