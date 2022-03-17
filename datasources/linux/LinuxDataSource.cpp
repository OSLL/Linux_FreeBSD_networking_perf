//
// Created by shenk on 17.12.2021.
//

#include <QDir>
#include "LinuxDataSource.h"

QMap<QString, QString> LinuxDataSource::protocol_sockets_files  = {
        {"tcp",      "/proc/net/tcp"},
        {"tcp6",     "/proc/net/tcp6"},
        {"udp",      "/proc/net/udp"},
        {"udp6",     "/proc/net/tcp6"},
        {"udplite",  "/proc/net/udplite"},
        {"udplite6", "/proc/net/udplite6"},
        {"raw",      "/proc/net/raw"},
        {"raw6",     "/proc/net/raw6"},
        {"icmp",     "/proc/net/icmp"},
        {"icmp6",    "/proc/net/icmp6"}
};

QMap<QString, std::tuple<std::optional<QString>, std::optional<QString>>> LinuxDataSource::protocol_stats_names = {
        {"ip", {"Ip", "IpExt"}},
        {"icmp", {"Icmp", std::nullopt}},
        //TODO: Не похоже, что это отдельный протокол. Нужно объединить с icmp?
        {"icmpmsg", {"IcmpMsg", std::nullopt}},
        {"tcp", {"Tcp", "TcpExt"}},
        {"udp", {"Udp", std::nullopt}},
        {"udplite", {"UdpLite", std::nullopt}},
        {"mptcp", {std::nullopt, "MPTcpExt"}}
};

std::optional<ProtocolStats> LinuxDataSource::getProtocolStats(const QString &protocol) {

    if (protocol.endsWith('6')) {
        return this->_getProtocolV6Stats(protocol);
    } else {
        return this->_getProtocolStats(protocol);
    }

}

QVector<SocketInfo> LinuxDataSource::getSockets(QString protocol) {

    QVector<SocketInfo> sockets_info_list;

    auto iter = LinuxDataSource::protocol_sockets_files.find(protocol);

    if (iter == LinuxDataSource::protocol_sockets_files.end()) {
        std::cout << "Unsupported protocol" << std::endl;
        return sockets_info_list;
    }

    auto filename = iter.value();
    auto sockets_list = parseProtocolSocketsListFile(filename);

    if (!sockets_list || sockets_list.value().empty()) {
        return sockets_info_list;
    }

    // В файлах для ipv4 и ipv6 используются разные заголовки для удаленного адреса
    auto remote_address_key = protocol.endsWith('6') ?  "remote_address" : "rem_address";

    for (auto &socket: *sockets_list) {

        auto v_loc_addr_port = socket["local_address"].split(':', Qt::SkipEmptyParts);
        auto v_for_addr_port = socket[remote_address_key].split(':', Qt::SkipEmptyParts);

        char loc_addr[INET_ADDRSTRLEN];
        char for_addr[INET_ADDRSTRLEN];

        auto i_loc_addr = v_loc_addr_port[0].toUInt(nullptr, 16);
        auto loc_port = v_loc_addr_port[1].toUInt(nullptr, 16);
        inet_ntop(AF_INET, &i_loc_addr, loc_addr, INET_ADDRSTRLEN);

        auto i_for_addr = v_for_addr_port[0].toUInt(nullptr, 16);
        auto for_port = v_for_addr_port[1].toUInt(nullptr, 16);
        inet_ntop(AF_INET, &i_for_addr, for_addr, INET_ADDRSTRLEN);

        auto rx_queue = socket["rx_queue"].toUInt();
        auto tx_queue = socket["tx_queue"].toUInt();

        auto ref = socket["ref"].toInt();

        // В /proc/net/tcp нет информации о сброшенных пакетах. Предполагаю, что это из=за того, что tcp контролирует
        // доставку всей информации
        int drops = protocol == "tcp" ? 0 : socket["drops"].toInt();

        sockets_info_list.push_back({
                                            QString(loc_addr), QString(for_addr), loc_port, for_port,
                                            rx_queue, tx_queue, ref, drops
                                    });

    }

    return sockets_info_list;
}

std::optional<QMap<QString, int>> LinuxDataSource::_getProtocolStats(const QString &protocol) {

    if (!LinuxDataSource::protocol_stats_names.contains(protocol)) {
        std::cout << "Unsupported protocol" << std::endl;
        return std::nullopt;
    }

    auto o_protocols_stats = parseProtocolsStatsFile("/proc/net/snmp");
    auto o_protocols_stats_ext = parseProtocolsStatsFile("/proc/net/netstat");

    if (!o_protocols_stats || !o_protocols_stats_ext) return std::nullopt;

    auto protocols_stats = o_protocols_stats.value();
    auto protocols_stats_ext = o_protocols_stats_ext.value();

    auto [o_stat_name, o_ext_stat_name] = LinuxDataSource::protocol_stats_names.value(protocol);

    if (
            (o_stat_name && !protocols_stats.contains(*o_stat_name)) ||
            (o_ext_stat_name && !protocols_stats_ext.contains(*o_ext_stat_name)))
    {
        std::cout << "No protocol data in protocols stats file" << std::endl;
        return std::nullopt;
    }

    if (o_stat_name && o_ext_stat_name) {
        auto protocol_stats = protocols_stats.value(*o_stat_name);
        auto protocol_stats_ext = protocols_stats_ext.value(*o_ext_stat_name);

        for (auto it = protocol_stats_ext.begin(); it != protocol_stats_ext.end(); it++) {
            protocol_stats[it.key()] = it.value();
        }

        return protocol_stats;
    } else if (o_stat_name) {
        return protocols_stats.value(*o_stat_name);
    } else if (o_protocols_stats_ext) {
        return protocols_stats_ext.value(*o_ext_stat_name);
    }

    return std::nullopt;

}

std::optional<QMap<QString, int>> LinuxDataSource::_getProtocolV6Stats(const QString &protocol) {

    auto o_protocols_stats = parseProtocolsV6StatsFile("/proc/net/snmp6");
    if (!o_protocols_stats) return std::nullopt;

    auto protocols_stats = o_protocols_stats.value();

    if (!protocols_stats.contains(protocol)) {
        std::cout << "Unsupported protocol" << std::endl;
        return std::nullopt;
    }

    return protocols_stats.value(protocol);

}

std::optional<CpusSoftnetData> LinuxDataSource::getSoftnetData() {
    return parseSoftnetDataFile("/proc/net/softnet_stat");
}

#define PROCESSED_COUNT_INDEX 0
#define CPU_NUMBER_INDEX 12
std::optional<QMap<int, int>> LinuxDataSource::getCPUDistribution(CPUDistributionSource source) {

    if (source == SOFTNET_PROCESSED) {

        QMap<int, int> softnet_processed;
        auto o_softnet_data = parseSoftnetDataFile("/proc/net/softnet_stat");
        if (!o_softnet_data) return std::nullopt;

        auto softnet_data = o_softnet_data.value();

        for (const auto &sd: softnet_data) {
            softnet_processed[sd[CPU_NUMBER_INDEX]] = sd[PROCESSED_COUNT_INDEX];
        }
        return softnet_processed;

    } else {

        auto o_softirqs_count = parseSoftirqFile("/proc/softirqs");
        if (!o_softirqs_count) return std::nullopt;

        auto softirqs_count = o_softirqs_count.value();

        QString softirq_name = source == SOFTIRQ_NET_RX ? "NET_RX" : "NET_TX";

        if (!softirqs_count.contains(softirq_name)) {
            std::cout << "No " << softirq_name.toStdString() << " line in /proc/softirqs" << std::endl;
            return std::nullopt;
        }

        return softirqs_count.value(softirq_name);
    }
}

void LinuxDataSource::setRecvSockOpt(Socket &sock) {

    unsigned int val = SOF_TIMESTAMPING_RX_HARDWARE |
                       SOF_TIMESTAMPING_RX_SOFTWARE |
                       SOF_TIMESTAMPING_SOFTWARE |
                       SOF_TIMESTAMPING_RAW_HARDWARE;

    sock.setOpt(SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(unsigned int));

}

void LinuxDataSource::processRecvTimestamp(msghdr &msg, ReceiveTimestamp &res, timespec &after_recv_time,
                                           const Socket &sock) {

    for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {

        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMPING) {

            auto tmst = (scm_timestamping*) &CMSG_DATA(cmsg);

            if (!is_timespec_empty(tmst->ts[0])) {
                res.software_recv = tmst->ts[0];
            }
            if (!is_timespec_empty(tmst->ts[2])) {
                res.hardware_recv =tmst->ts[2];
            }
        }
    }

}

void LinuxDataSource::setSendSockOpt(Socket &sock, const MeasureType measure_type) {

    unsigned int val;

    switch (measure_type) {
        case SCHEDULE:
            val = SOF_TIMESTAMPING_TX_SCHED | SOF_TIMESTAMPING_SOFTWARE;
            break;
        case SOFTWARE:
            val = SOF_TIMESTAMPING_TX_SOFTWARE | SOF_TIMESTAMPING_SOFTWARE;
            break;
        case HARDWARE:
            val = SOF_TIMESTAMPING_TX_HARDWARE | SOF_TIMESTAMPING_RAW_HARDWARE;
            break;
        case ACK:
            val = SOF_TIMESTAMPING_TX_ACK | SOF_TIMESTAMPING_SOFTWARE;
            break;
    }

    val |= SOF_TIMESTAMPING_OPT_ID;
    sock.setOpt(SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(val));
}

void LinuxDataSource::processSendTimestamp(Socket &sock, SendTimestamp &res, TimeRange &timestamps) {

    auto protocol = sock.getProtocol();
    if (protocol.startsWith("sctp") || protocol.startsWith("mptcp")) return;

    char control[1000];
    memset(control, 0, sizeof(control));
    msghdr msg {
            .msg_name = nullptr,
            .msg_namelen = 0,
            .msg_iov = nullptr,
            .msg_iovlen = 0,
            .msg_control = &control,
            .msg_controllen = sizeof(control)
    };

    scm_timestamping *tmst = nullptr;

    // Далее происходит работа с очередью ошибок. Так как она всегда асинхронна, то receiveMsg не заблокируется.
    // TODO: использовать poll или pselect
    int is_cmsg_exist;
    do {
        is_cmsg_exist = sock.receiveMsg(msg, MSG_ERRQUEUE);
    } while (is_cmsg_exist < 0);

    for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMPING) {
            tmst = (scm_timestamping *) &CMSG_DATA(cmsg);
        }
    }

    if (tmst) {
        if (!is_timespec_empty(tmst->ts[0])) {
            res.software_send = tmst->ts[0];
        }
        if (!is_timespec_empty(tmst->ts[2])) {
            res.hardware_send = tmst->ts[2];
        }
    }
}

std::optional<QMap<QString, DropsInfo>> LinuxDataSource::getDevsDropsInfo() {

    QMap<QString, DropsInfo> drops_info;
    QDir dir("/sys/class/net");

    if (!dir.exists()) {
        std::cout << "No /sys/class/net folder" << std::endl;
        return std::nullopt;
    }

    for (const auto &entry: dir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs)) {

        auto rx_drops = get_int_from_file("/sys/class/net/" + entry + "/statistics/rx_dropped");
        auto tx_drops = get_int_from_file("/sys/class/net/" + entry + "/statistics/tx_dropped");

        if (rx_drops && tx_drops) {
            drops_info[entry] = DropsInfo(*rx_drops, *tx_drops);
        }
    }

    return drops_info;
}

QVector<QPair<QString, DropsInfo>> LinuxDataSource::getDropsInfo() {

    QVector<QPair<QString, DropsInfo>> drops_info;
    auto dev_drops_info = getDevsDropsInfo();
    if (dev_drops_info) {
        for (auto it = dev_drops_info->begin(); it != dev_drops_info->end(); it++) {
            drops_info.push_back({it.key(), it.value()});
        }
    }

    auto softnet_data = getSoftnetData();
    if (softnet_data) {
        int poll = 0;
        for (const auto &cpu_sd: *softnet_data) {
            poll += cpu_sd[1];
        }
        drops_info.push_back({"poll", DropsInfo(poll)});
    }

    auto ip_stats = getProtocolStats("ip");
    if (ip_stats) {
        drops_info.push_back({"ip", DropsInfo(ip_stats->value("InDiscards"), ip_stats->value("OutDiscards"))});
    }

    auto udp_stats = getProtocolStats("udp");
    if (udp_stats) {
        drops_info.push_back({"udp", DropsInfo(ip_stats->value("RcvbufErrors"), ip_stats->value("SndbufErrors:"))});
    }

    auto udplite_stats = getProtocolStats("udplite");
    if (udplite_stats) {
        drops_info.push_back({"udplite", DropsInfo(ip_stats->value("RcvbufErrors"), ip_stats->value("SndbufErrors:"))});
    }

    return drops_info;
}

QStringList LinuxDataSource::getSupportedSocketsListProtocols() {
    return LinuxDataSource::protocol_sockets_files.keys();
}

QStringList LinuxDataSource::getSupportedStatsProtocols() {
    return LinuxDataSource::protocol_stats_names.keys();
}

std::optional<FuncProfilerTreeNode*> LinuxDataSource::getProfilerData() {
    return parseProfilerData("/dev/netprofiler");
}
