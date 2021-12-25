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

int LinuxDataSource::getTcpTotalRecv() {

    auto protocols_stats = parseProtocolsStatsFile("/proc/net/snmp");
    if (protocols_stats) {
        return protocols_stats.value()["Tcp"]["InSegs"];
    }
    return 0;

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

        sockets_info_list.emplace_back(
                std::string(loc_addr), std::string(for_addr), loc_port, for_port, rx_queue, tx_queue
        );

    }

    return sockets_info_list;
}

std::optional<InSystemTimeRXInfo>
LinuxDataSource::getInSystemTimeRX(const QString &protocol, unsigned int port, unsigned int packets_count) {

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock < 0) {
        std::cout << "Could not create socket" << std::endl;
    }

    unsigned int val = SOF_TIMESTAMPING_RX_HARDWARE |
            SOF_TIMESTAMPING_RX_SOFTWARE |
            SOF_TIMESTAMPING_SOFTWARE |
            SOF_TIMESTAMPING_RAW_HARDWARE;

    setsockopt(sock, SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(val));

    if (protocol != "raw") {

        sockaddr_in addr {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = {
                    .s_addr = INADDR_ANY
            }
        };

        if (bind(sock, (sockaddr *)&addr, sizeof(addr)) < 0) {
            std::cout << "Bind failed" << std::endl;
            return std::nullopt;
        }

        listen(sock, 1);

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

    InSystemTimeRXInfo res;

    scm_timestamping *tmst;
    timespec user_time;

    for (int i=0; i<packets_count; i++) {

        std::cout << "Waiting for data" << std::endl;
        recvmsg(sock, &msg, 0);
        clock_gettime(CLOCK_REALTIME, &user_time);

        for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {

            if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMPING) {

                tmst = (scm_timestamping*) &CMSG_DATA(cmsg);

                timespec_avg_add(res.rx_kernel_user_time, tmst->ts[0], user_time, packets_count);
                timespec_avg_add(res.rx_hardware_kernel_time, tmst->ts[2], user_time, packets_count);
            }
        }
    }

    return res;
}

std::optional<InSystemTimeTXInfo> LinuxDataSource::sendTimestamp(const QString &protocol, const QString &ip_addr, unsigned int port, unsigned int packets_count) {

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    unsigned int val = SOF_TIMESTAMPING_TX_HARDWARE |
                       SOF_TIMESTAMPING_TX_SOFTWARE |
//                       SOF_TIMESTAMPING_TX_SCHED |
                       SOF_TIMESTAMPING_SOFTWARE |
                       SOF_TIMESTAMPING_RAW_HARDWARE;

    setsockopt(sock, SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(val));

    sockaddr_in addr {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };
    inet_aton(ip_addr.toLocal8Bit().data(), &addr.sin_addr);

    if (connect(sock, (sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cout << "Connect error" << std::endl;
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

    timespec res = {0, 0};

    for (int i=0; i<packets_count; i++) {

        timespec user_time = {0, 0};
        clock_gettime(CLOCK_REALTIME, &user_time);
        if (send(sock, &user_time, sizeof(timespec), 0) < 0) {
            std::cout << "Send error" << std::endl;
        };

        recvmsg(sock, &msg, MSG_ERRQUEUE);

        scm_timestamping *tmst;
        for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {

            if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMPING) {

                tmst = (scm_timestamping*) &CMSG_DATA(cmsg);

                std::cout << user_time.tv_sec << " " << user_time.tv_nsec << std::endl;
                for (auto &t : tmst->ts) {
                    std::cout << t.tv_sec << " " << t.tv_nsec << std::endl;
                }

                timespec_avg_add(res, user_time, tmst->ts[0], packets_count);

            }
        }
    }

    std::cout << res.tv_sec << " " << res.tv_nsec << std::endl;

    return std::nullopt;
}
