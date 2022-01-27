//
// Created by shenk on 17.12.2021.
//

#include <QDir>
#include "LinuxDataSource.h"

std::map<std::string, std::string> LinuxDataSource::protocol_sockets_files  = {
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

std::optional<QMap<QString, int>> LinuxDataSource::getProtocolStats(const QString &protocol) {

    if (protocol.endsWith('6')) {
        return this->_getProtocolV6Stats(protocol);
    } else {
        return this->_getProtocolStats(protocol);
    }

}

std::vector<SocketInfo> LinuxDataSource::getSockets(std::string protocol) {

    std::vector<SocketInfo> sockets_info_list;

    auto iter = LinuxDataSource::protocol_sockets_files.find(protocol);

    if (iter == LinuxDataSource::protocol_sockets_files.end()) {
        std::cout << "Unsupported protocol" << std::endl;
        return sockets_info_list;
    }

    auto filename = iter->second;
    auto sockets_list = parseProtocolSocketsListFile(filename);

    if (!sockets_list) {
        return sockets_info_list;
    }

    for (auto &socket: *sockets_list) {

        auto v_loc_addr_port = split(socket["local_address"], ':');
        auto v_for_addr_port = split(socket["rem_address"], ':');

        char loc_addr[INET_ADDRSTRLEN];
        char for_addr[INET_ADDRSTRLEN];

        unsigned int i_loc_addr = std::stol(v_loc_addr_port[0], nullptr, 16);
        unsigned int loc_port = std::stoi(v_loc_addr_port[1], nullptr, 16);
        inet_ntop(AF_INET, &i_loc_addr, loc_addr, INET_ADDRSTRLEN);

        unsigned int i_for_addr = std::stol(v_for_addr_port[0], nullptr, 16);
        unsigned int for_port = std::stoi(v_for_addr_port[1], nullptr, 16);
        inet_ntop(AF_INET, &i_for_addr, for_addr, INET_ADDRSTRLEN);

        int rx_queue = std::stoi(socket["rx_queue"]);
        int tx_queue = std::stoi(socket["tx_queue"]);

        int ref = std::stoi(socket["ref"]);

        // В /proc/net/tcp нет информации о сброшенных пакетах. Предполагаю, что это из=за того, что tcp контролирует
        // доставку всей информации
        int drops = protocol == "tcp" ? 0 : std::stoi(socket["drops"]);

        sockets_info_list.emplace_back(
                std::string(loc_addr), std::string(for_addr), loc_port, for_port,
                rx_queue, tx_queue, ref, drops
        );

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
                                           const QString &protocol) {

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

    if (measure_type == SCHEDULE) {
        val = SOF_TIMESTAMPING_TX_SCHED | SOF_TIMESTAMPING_SOFTWARE;
    } else if (measure_type == SOFTWARE) {
        val = SOF_TIMESTAMPING_TX_SOFTWARE | SOF_TIMESTAMPING_SOFTWARE;
    } else if (measure_type == HARDWARE) {
        val = SOF_TIMESTAMPING_TX_HARDWARE | SOF_TIMESTAMPING_RAW_HARDWARE;
    } else if (measure_type == ACK) {
        val = SOF_TIMESTAMPING_TX_ACK | SOF_TIMESTAMPING_SOFTWARE;
    } else {
        std::cout << "Unknown measure type, used software" << std::endl;
    }

    sock.setOpt(SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(val));
}

void LinuxDataSource::processSendTimestamp(Socket &sock, SendTimestamp &res, TimeRange &timestamps) {

    auto protocol = sock.getProtocol();
    if (!(protocol == "tcp" || protocol == "udp")) return;

    // Далее происходит работа с очередью ошибок. Так как в ней не гарантируется правильная последовательность пакетов
    // то возникают некоторые проблемы. 1. При обработке нужно найти timestamp для текущего времени. Он должен находится
    // между вызовом send и выхода из него (находятся в аргументе timestamps). 2. Иногда timestamp'ы просто исчезают и не удается
    // найти нужный, думаю, они отбрасываются при обработке прошлых пакетов. Так что здесь возвращается true/false.
    // Пакет обработан или нет. Если нет, то его так же не нужно учитывать во всех временах и значит делить нужно
    // не на packets_count. Это обрабатывается в sendTimestamps.

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

    bool is_valid_timestamp = false;
    while (!is_valid_timestamp) {
        auto is_cmsg_exist = sock.receiveMsgTS(msg, MSG_ERRQUEUE | MSG_WAITALL);
        if (!is_cmsg_exist) return;

        for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
            if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMPING) {
                tmst = (scm_timestamping *) &CMSG_DATA(cmsg);

                int check_index = 0; // Software timestamps
                if (is_timespec_empty(tmst->ts[0])) {
                    check_index = 2; // Hardware timestamps
                }
                is_valid_timestamp = timespeccmp(tmst->ts[check_index], timestamps.from) > 0 &&
                        timespeccmp(tmst->ts[check_index], timestamps.to) < 0;
            }
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
