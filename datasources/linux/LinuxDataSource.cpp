//
// Created by shenk on 17.12.2021.
//

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

//TODO: различные источники данных (NET_TX в /proc/softirq, /proc/net/softnet_stat)
std::optional<QMap<int, int>> LinuxDataSource::getCPUDistribution() {
    auto o_softirqs_count = parseSoftirqFile("/proc/softirqs");
    if (!o_softirqs_count) return std::nullopt;

    auto softirqs_count = o_softirqs_count.value();

    if (!softirqs_count.contains("NET_RX")) {
        std::cout << "No NET_RX line in /proc/softirqs" << std::endl;
        return std::nullopt;
    }

    return softirqs_count.value("NET_RX");
}

void LinuxDataSource::setRecvSockOpt(Socket &sock) {

    unsigned int val = SOF_TIMESTAMPING_RX_HARDWARE |
                       SOF_TIMESTAMPING_RX_SOFTWARE |
                       SOF_TIMESTAMPING_SOFTWARE |
                       SOF_TIMESTAMPING_RAW_HARDWARE;

    sock.setOpt(SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(unsigned int));

}

void LinuxDataSource::processRecvTimestamp(msghdr &msg, InSystemTimeInfo &res, timespec &after_recv_time,
                                           unsigned int packets_count, const QString &protocol) {

    for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {

        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMPING) {

            auto tmst = (scm_timestamping*) &CMSG_DATA(cmsg);

            timespec_avg_add(res.software_time, tmst->ts[0], after_recv_time, packets_count);
            timespec_avg_add(res.hardware_time, tmst->ts[2], after_recv_time, packets_count);

        }
    }

}

void LinuxDataSource::setSendSockOpt(Socket &sock, const QString &measure_type) {

    unsigned int val = SOF_TIMESTAMPING_TX_SOFTWARE | SOF_TIMESTAMPING_SOFTWARE;

    if (measure_type == "scheduler") {
        val |= SOF_TIMESTAMPING_TX_SCHED;
    } else if (measure_type != "software"){
        std::cout << "Unknown measure type, used software" << std::endl;
    }

    sock.setOpt(SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(val));
}

bool LinuxDataSource::processSendTimestamp(Socket &sock, InSystemTimeInfo &res,
        SocketOpTimestamps &timestamps, unsigned int packets_count, const QString &protocol) {

    if (!(protocol == "tcp" || protocol == "udp")) return true;

    // Далее происходит работа с очередью ошибок. Так как в ней не гарантируется правильная последовательность пакетов
    // то возникают некоторые проблемы. 1. При обработке нужно найти timestamp для текущего времени. Он должен находится
    // между вызовом send и выхода из него (в аргументе timestamps). 2. Иногда timestamp'ы просто исчезают и не удается
    // найти нужный, причину найти не смог. Так что здесь возвращается true/false. Пакет обработан или нет. Если нет, то
    // его так же не нужно учитывать во всех временах и значит делить нужно не на packets_count. Это обрабатывается в
    // sendTimestamp.

    char control[1000];
    memset(control, 0, sizeof(control));
    msghdr msg{
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
        auto is_cmsg_exist = sock.receiveMsg(msg, MSG_ERRQUEUE | MSG_WAITALL);
        if (!is_cmsg_exist) return false;

        for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
            if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMPING) {
                tmst = (scm_timestamping *) &CMSG_DATA(cmsg);

                is_valid_timestamp = timespeccmp(tmst->ts[0], timestamps.before_op_time) > 0 &&
                        timespeccmp(tmst->ts[0], timestamps.after_op_time) < 0;
            }
        }
    }

    timespec_avg_add(res.software_time, timestamps.before_op_time, tmst->ts[0], packets_count);
    timespec_avg_add(res.hardware_time, timestamps.before_op_time, tmst->ts[2], packets_count);

    return true;
}
