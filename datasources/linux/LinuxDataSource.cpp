//
// Created by shenk on 17.12.2021.
//

#include "LinuxDataSource.h"
#include "../../utils/sockets.h"

#include "QDebug"

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

std::optional<TcpStats> LinuxDataSource::getTcpStats() {

    auto o_protocols_stats = parseProtocolsStatsFile("/proc/net/snmp");
    auto o_protocols_stats_ext = parseProtocolsStatsFile("/proc/net/netstat");

    if (!o_protocols_stats || !o_protocols_stats_ext) {
        std::cout << "Can't open stats files" << std::endl;
        return std::nullopt;
    }

    auto protocol_stats = *o_protocols_stats;
    auto protocol_stats_ext = *o_protocols_stats_ext;

    if (!protocol_stats.contains("Tcp") || !protocol_stats_ext.contains("TcpExt")) {
        std::cout << "No TCP in protocols stats file" << std::endl;
        return std::nullopt;
    }

    auto tcp_stats = protocol_stats["Tcp"];
    auto tcp_stats_ext = protocol_stats_ext["TcpExt"];

    return TcpStats {

        .syncookies_sent = tcp_stats_ext["SyncookiesSent"],
        .syncookise_recv = tcp_stats_ext["SyncookiesRecv"]

    };

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

//TODO: Проверить hardware timestamps
std::optional<InSystemTimeInfo>
LinuxDataSource::recvTimestamp(const QString &protocol, unsigned int port, unsigned int packets_count) {

    Socket sock(protocol);

    unsigned int val = SOF_TIMESTAMPING_RX_HARDWARE |
            SOF_TIMESTAMPING_RX_SOFTWARE |
            SOF_TIMESTAMPING_SOFTWARE |
            SOF_TIMESTAMPING_RAW_HARDWARE;

    sock.setOpt(SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(unsigned int));

    if (sock.bindTo(INADDR_ANY, port) < 0) {
        std::cout << "Bind failed" << std::endl;
        return std::nullopt;
    }

    if (sock.listenFor(1) < 0) {
        std::cout << "Listen failed" << std::endl;
        return std::nullopt;
    }

    char control[1000];
    timespec send_time;

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

    InSystemTimeInfo res;

    scm_timestamping *tmst;
    timespec user_time;

    for (int i=0; i<packets_count; i++) {

        sock.receiveMsg(msg, 0);

        clock_gettime(CLOCK_REALTIME, &user_time);

        for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {

            if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMPING) {

                tmst = (scm_timestamping*) &CMSG_DATA(cmsg);

                timespec_avg_add(res.software_time, tmst->ts[0], user_time, packets_count);
                timespec_avg_add(res.hardware_time, tmst->ts[2], user_time, packets_count);
                timespec_avg_add(res.total_time, send_time, user_time, packets_count);
            }
        }
    }

    return res;
}


//TODO: Отправка в raw сокет
std::optional<InSystemTimeInfo>
LinuxDataSource::sendTimestamp(
        const QString &protocol,
        const QString &addr,
        unsigned int port,
        unsigned int packets_count,
        const QString& measure_type) {

    unsigned int val = SOF_TIMESTAMPING_TX_HARDWARE |
                       SOF_TIMESTAMPING_SOFTWARE |
                       SOF_TIMESTAMPING_RAW_HARDWARE;

    if (measure_type == "software") {
        val |= SOF_TIMESTAMPING_TX_SOFTWARE;
    } else if (measure_type == "scheduler") {
        val |= SOF_TIMESTAMPING_TX_SCHED;
    } else {
        std::cout << "Unknown measure type" << std::endl;
        return std::nullopt;
    }

    Socket sock(protocol);
    sock.setOpt(SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(val));

    if (sock.connectTo(addr, port) < 0) {
        std::cout << "Connect error" << std::endl;
        return std::nullopt;
    }

    char control[1000];
    msghdr msg{
            .msg_name = nullptr,
            .msg_namelen = 0,
            .msg_iov = nullptr,
            .msg_iovlen = 0,
            .msg_control = &control,
            .msg_controllen = sizeof(control)
    };

    InSystemTimeInfo res;

    for (int i=0; i<packets_count; i++) {

        timespec user_time = {0, 0};
        clock_gettime(CLOCK_REALTIME, &user_time);

        if (sock.sendData(&user_time, sizeof(timespec)) < 0) {
            std::cout << "Send error" << std::endl;
            return std::nullopt;
        };

        if (!(protocol == "tcp" || protocol == "udp")) continue;

        //TODO: Иногда возвращается значение из прошлой итерации, из-за этого получается, что user-time (время, когда
        // отправили из user-space) больше чем software-time (время, когда покинуло ядро) -> переполнение при вычитании
        sock.receiveMsg(msg, MSG_ERRQUEUE);

        scm_timestamping *tmst;
        for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {

            if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMPING) {

                tmst = (scm_timestamping*) &CMSG_DATA(cmsg);

                std::cout << "UT: " << user_time.tv_sec << " " << user_time.tv_nsec << std::endl;
                std::cout << "ST: " << tmst->ts[0].tv_sec << " " << tmst->ts[0].tv_nsec << std::endl;
                std::cout << "HT: " << tmst->ts[2].tv_sec << " " << tmst->ts[2].tv_nsec << std::endl;

                timespec_avg_add(res.software_time, user_time, tmst->ts[0], packets_count);
                timespec_avg_add(res.hardware_time, user_time, tmst->ts[2], packets_count);

            }
        }
    }

    return res;
}
