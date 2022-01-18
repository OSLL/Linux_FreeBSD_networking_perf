//
// Created by shenk on 17.12.2021.
//

#include "FreeBSDDataSource.h"
#include <QDebug>

std::map<std::string, std::string> FreeBSDDataSource::protocol_sockets_sysctl_names = {
        {"tcp", "net.inet.tcp.pcblist"},
        {"udp", "net.inet.udp.pcblist"},
        {"raw", "net.inet.raw.pcblist"}
};

//TODO: icmp6 и ip6
QMap<QString, std::tuple<QString, size_t>> FreeBSDDataSource::protocol_stats_sysctl_names = {
        {"tcp", {"net.inet.tcp.stats", sizeof(tcpstat)}},
        {"udp", {"net.inet.udp.stats", sizeof(udpstat)}},
        {"icmp", {"net.inet.icmp.stats", sizeof(icmpstat)}},
        {"igmp", {"net.inet.igmp.stats", sizeof(igmpstat)}},
        {"ip", {"net.inet.ip.stats", sizeof(ipstat)}},
};

QMap<QString, QVector<QString>> FreeBSDDataSource::protocols_stats_descriptions =
#include "protocols_stats_descriptions.h"

std::optional<QMap<QString, int>> FreeBSDDataSource::getProtocolStats(const QString &protocol) {

    if (!FreeBSDDataSource::protocol_stats_sysctl_names.contains(protocol)) {
        std::cout << "Unsupported protocol" << std::endl;
        return std::nullopt;
    }

    auto [sysctl_name, size] = FreeBSDDataSource::protocol_stats_sysctl_names.value(protocol);
    size /= sizeof(uint64_t);

    QVector<uint64_t> protocol_stats_vals(size);

    sysctlbyname(sysctl_name.toLocal8Bit().data(), protocol_stats_vals.data(), &size, nullptr, 0);

    auto protocol_stats_names = FreeBSDDataSource::protocols_stats_descriptions.value(protocol);
    QMap<QString, int> protocol_stats;

    for (int i=0; i<protocol_stats_names.size(); i++) {
        protocol_stats[protocol_stats_names[i]] = protocol_stats_vals[i];
    }

    return protocol_stats;
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
                so->so_rcv.sb_cc, so->so_snd.sb_cc,
                0, 0
        );
    }
    return sockets_info_list;
}

//TODO: Проработать netist, там много полезного. Например, поле qdrops.
std::optional<QMap<int, int>> FreeBSDDataSource::getCPUDistribution() {

    size_t size;
    sysctlbyname("net.isr.work", nullptr, &size, nullptr, 0);

    sysctl_netisr_work snw_array[size/sizeof(sysctl_netisr_work)];
    sysctlbyname("net.isr.work", snw_array, &size, nullptr, 0);

    // Судя по коду FreeBSD (sys/net/netisr.c:sysctl_netisr_work) cpuid всегда будут идти подряд. Но на всякий случай,
    // реализовал через map.
    QMap<int, int> cpus_dist;
    for (const auto& snw: snw_array) {

        if (!cpus_dist.contains(snw.snw_wsid)) {
            cpus_dist[snw.snw_wsid] = snw.snw_handled;
        } else {
            cpus_dist[snw.snw_wsid] += snw.snw_handled;
        }

    }

    return cpus_dist;
}

void FreeBSDDataSource::setRecvSockOpt(Socket &sock) {

    if (sock.getProtocol() == "udp") {
        int val = 1;
        sock.setOpt(SOL_SOCKET, SO_TIMESTAMP, &val, sizeof(val));
    }

}

void FreeBSDDataSource::processRecvTimestamp(msghdr &msg, InSystemTimeInfo &res, timespec &after_recv_time,
                                             unsigned int packets_count, const QString &protocol) {

    if (protocol == "udp") {
        for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
            if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMP) {
                auto tmst = (timespec *) CMSG_DATA(cmsg);

                // Во FreeBSD здесь хранятся микросекунды, но clock_gettime возвращает наносекунды.
                tmst->tv_nsec *= 1000;
                timespec_avg_add(res.software_time, *tmst, after_recv_time, packets_count);
            }
        }
    }

}

void FreeBSDDataSource::setSendSockOpt(Socket &sock, const QString &measure_type) {
    // Во FreeBSD не реализована получение timestamp'ов при получении
    return;
}

void
FreeBSDDataSource::processSendTimestamp(Socket &sock, msghdr &msg, InSystemTimeInfo &res, timespec &before_send_time,
                                        unsigned int packets_count, const QString &protocol, timespec &prev) {
    // Во FreeBSD не реализована получение timestamp'ов при получении
    return;
}
