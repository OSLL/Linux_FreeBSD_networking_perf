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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

InSystemTimeInfo LinuxDataSource::getInSystemTime() {

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

    if (sock < 0) {
        std::cout << "Could not create socket" << std::endl;
    }

    int val = 1;
    int res = setsockopt(sock, SOL_SOCKET, SO_TIMESTAMP, &val, sizeof(val));
//
//    sockaddr_in addr{};
//
//    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr = INADDR_ANY;
//    addr.sin_port = 7435;
//
//    if (bind(sock, (sockaddr *)&addr, sizeof(addr)) < 0) {
//        std::cout << "Bind failed" << std::endl;
//    }

    char control[1000];

    msghdr msg{
            .msg_name = nullptr,
            .msg_namelen = 0,
            .msg_iov = nullptr,
            .msg_iovlen = 0,
            .msg_control = &control,
            .msg_controllen = sizeof(control)
    };

    recvmsg(sock, &msg, 0);

    timespec user_time, diff_time;
    clock_gettime(CLOCK_REALTIME, &user_time);

    for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {

        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMP) {

            auto *kernel_time = (timespec*) &CMSG_DATA(cmsg);
            diff_time = timespecsub(user_time, *kernel_time);

        }

    }

    std::cout << "Seconds: " << diff_time.tv_sec << std::endl << "Nanoseconds: " << diff_time.tv_nsec << std::endl;

}
