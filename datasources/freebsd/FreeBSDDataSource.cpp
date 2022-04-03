//
// Created by shenk on 17.12.2021.
//

#include "FreeBSDDataSource.h"
#include <QDebug>

QMap<QString, QString> FreeBSDDataSource::protocol_sockets_sysctl_names = {
        {"tcp", "net.inet.tcp.pcblist"},
        {"udp", "net.inet.udp.pcblist"},
        {"raw", "net.inet.raw.pcblist"}
};

//TODO: icmp6 и ip6
QMap<QString, std::tuple<QString, size_t, std::function<ProtocolStats(char*)>>> FreeBSDDataSource::protocol_stats_sysctl_names = {
        {"ip", {"net.inet.ip.stats", sizeof(ipstat), &ip_stats}},
        {"tcp", {"net.inet.tcp.stats", sizeof(tcpstat), &tcp_stats}},
        {"udp", {"net.inet.udp.stats", sizeof(udpstat), &udp_stats}},
        {"icmp", {"net.inet.icmp.stats", sizeof(icmpstat), &icmp_stats}},
        {"igmp", {"net.inet.igmp.stats", sizeof(igmpstat), &igmp_stats}},
        {"ip6", {"net.inet6.ip6.stats", sizeof(ip6stat), &ip6_stats}},
        {"raw6", {"net.inet6.ip6.rip6stats", sizeof(rip6stat), &rip6_stats}},
        {"icmp6", {"net.inet6.icmp6.stats", sizeof(icmp6stat), &icmp6_stats}},
};

std::optional<QMap<QString, int>> FreeBSDDataSource::getProtocolStats(const QString &protocol) {

    if (!FreeBSDDataSource::protocol_stats_sysctl_names.contains(protocol)) {
        std::cout << "Unsupported protocol" << std::endl;
        return std::nullopt;
    }

    auto [sysctl_name, size, stats_func] = FreeBSDDataSource::protocol_stats_sysctl_names.value(protocol);

    auto protocol_stats = new char[size];

    sysctlbyname(sysctl_name.toLocal8Bit().data(), protocol_stats, &size, nullptr, 0);

    return stats_func(protocol_stats);
}

QVector<SocketInfo> FreeBSDDataSource::getSockets(QString protocol) {

    size_t size;
    QVector<SocketInfo> sockets_info_list;

     auto iter = FreeBSDDataSource::protocol_sockets_sysctl_names.find(protocol);

    if (iter == FreeBSDDataSource::protocol_sockets_sysctl_names.end()) {
        std::cout << "Unsupported protocol" << std::endl;
        return sockets_info_list;
    }
    auto sysctl_name = iter.value();

    sysctlbyname(sysctl_name.toLocal8Bit().data(), nullptr, &size, nullptr, 0);

    char *buf = new char[size];
    sysctlbyname(sysctl_name.toLocal8Bit().data(), buf, &size, nullptr, 0);

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

        sockets_info_list.push_back({
                                            QString(loc_addr), QString(for_addr),
                                            ntohs(inp->inp_lport), ntohs(inp->inp_fport),
                                            so->so_rcv.sb_cc, so->so_snd.sb_cc,
                                            0, 0
                                    });
    }
    return sockets_info_list;
}

//TODO: Проработать netist, там много полезного. Например, поле qdrops.
std::optional<QMap<int, int>> FreeBSDDataSource::getCPUDistribution(CPUDistributionSource source) {

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

    if (sock.getType() == SOCK_DGRAM) {
        int val = 1;
        sock.setOpt(SOL_SOCKET, SO_TIMESTAMP, &val, sizeof(val));
    }

}

void FreeBSDDataSource::processRecvTimestamp(msghdr &msg, ReceiveTimestamp &res, timespec &after_recv_time,
                                             const Socket &sock) {

    if (sock.getType() == SOCK_DGRAM) {
        for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
            if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMP) {
                auto tmst = (timespec *) CMSG_DATA(cmsg);

                // Во FreeBSD здесь хранятся микросекунды, но clock_gettime возвращает наносекунды.
                tmst->tv_nsec *= 1000;
                res.software_recv = *tmst;
            }
        }
    }
}

void FreeBSDDataSource::setSendSockOpt(Socket &sock,  const MeasureType measure_type) {
    // Во FreeBSD не реализована получение timestamp'ов при получении
    return;
}

void
FreeBSDDataSource::processSendTimestamp(Socket &sock, SendTimestamp &res, TimeRange &timestamps) {
    // Во FreeBSD не реализована получение timestamp'ов при получении
    return;
}

std::optional<QMap<QString, DropsInfo>> FreeBSDDataSource::getDevsDropsInfo() {

    QMap<QString, DropsInfo> drops_info;

    ifaddrs *ifs;
    QSet<QString> interfaces;

    if (getifaddrs(&ifs) != 0) {
        std::cout << "Can't get interfaces info" << std::endl;
        return std::nullopt;
    }

    auto ifa = ifs;
    while (ifa) {

        if_data *data = (if_data *)ifa->ifa_data;
        QString if_name = ifa->ifa_name;
        if (data) {
            if (drops_info.contains(if_name)) {
                drops_info[if_name].rx_drops += data->ifi_iqdrops;
                drops_info[if_name].tx_drops += data->ifi_oqdrops;
            } else {
                drops_info[if_name] = DropsInfo(data->ifi_iqdrops, data->ifi_oqdrops);
            }
        }

        ifa = ifa->ifa_next;
    }

    freeifaddrs(ifs);

    return drops_info;
}

QVector<QPair<QString, DropsInfo>> FreeBSDDataSource::getDropsInfo() {

    QVector<QPair<QString, DropsInfo>> drops_info;

    auto dev_drops_info = getDevsDropsInfo();
    if (dev_drops_info) {
        for (auto it = dev_drops_info->begin(); it != dev_drops_info->end(); it++) {
            drops_info.push_back({it.key(), it.value()});
        }
    }

    size_t size;
    sysctlbyname("net.isr.work", nullptr, &size, nullptr, 0);

    sysctl_netisr_work snw_array[size/sizeof(sysctl_netisr_work)];
    sysctlbyname("net.isr.work", snw_array, &size, nullptr, 0);

    int poll_drops = 0;
    for (const auto &snw: snw_array) {
        poll_drops += snw.snw_qdrops;
    }

    drops_info.push_back({"poll", DropsInfo(poll_drops)});

    auto [udp_stats_name, udp_stats_size, _] = FreeBSDDataSource::protocol_stats_sysctl_names["udp"];
    udpstat udp_stats;
    sysctlbyname(udp_stats_name.toLocal8Bit().data(), &udp_stats, &udp_stats_size, nullptr, 0);

    drops_info.push_back({"udp", DropsInfo(udp_stats.udps_fullsock)});

    return drops_info;
}

QStringList FreeBSDDataSource::getSupportedSocketsListProtocols() {
    return FreeBSDDataSource::protocol_sockets_sysctl_names.keys();
}

QStringList FreeBSDDataSource::getSupportedStatsProtocols() {
    return FreeBSDDataSource::protocol_stats_sysctl_names.keys();
}

std::optional<ProfilerParser> FreeBSDDataSource::getProfilerData() {

    qDebug() << "Starting...";
    auto file = dTrace.start();
    qDebug() << "Exit start...";
    QTextStream stream(file.get());
    return ProfilerParser(stream);
}
#undef PROFILER