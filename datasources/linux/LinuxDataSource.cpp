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

    auto protocols_stats = this->parseProtocolsStatsFile("/proc/net/snmp");
    if (protocols_stats) {
        return protocols_stats.value()["Tcp"]["InSegs"];
    }
    return 0;

}

std::vector<SocketInfo> LinuxDataSource::getSockets(std::string protocol) {

    std::vector<SocketInfo> sockets_info_list;

    auto filename = LinuxDataSource::protocol_sockets_files[protocol];
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

        std::cout << loc_addr << ":" << loc_port << " -> " << for_addr << ":" << for_port << " " << tx_queue;
        std::cout << " " << rx_queue << std::endl;
    }

    return sockets_info_list;
}


std::optional<ProtocolsStats> LinuxDataSource::parseProtocolsStatsFile(std::string filename) {

    ProtocolsStats stats;

    std::ifstream file(filename);

    if (!file) {
        std::cout << "Can't open /proc/net/snmp";
        return std::nullopt;
    }

    std::string line;
    bool is_header = true;

    std::string protocol_name;
    std::vector<std::string> header;

    while (getline(file, line)) {

        auto v = split(line, ':');
        protocol_name = v[0];

        if (is_header) {
            header = split(v[1], ' ');

            // На следующей строке - информация
            is_header = false;

        } else {

            auto string_data = split(v[1], ' ');

            std::vector<int> data;
            data.reserve(string_data.size());

            for (const auto& s: string_data) {
                data.push_back(std::stoi(s));
            }

            std::map<std::string, int> protocol_stats;

            for (int i=0; i<header.size(); i++) {
                protocol_stats[header[i]] = data[i];
            }

            stats[protocol_name] = protocol_stats;

            // На следующей строке - заголовок
            is_header = true;
        }

    }

    return stats;

}

std::optional<SocketsList> LinuxDataSource::parseProtocolSocketsListFile(std::string filename) {

    std::vector<std::map<std::string, std::string>> sockets_list;

    std::ifstream file(filename);

    if (!file) {
        std::cout << "Can't open /proc/net/snmp";
        return std::nullopt;
    }

    std::string line;
    getline(file, line);

    auto header = split(line, ' ');

    while (getline(file, line)) {
        auto values = split(line, ' ');

        std::map<std::string, std::string> socket;

        int header_index = 0, value_index = 0;

        // В файлах этого типа могут встречаться "сдвоенные значения", содержащие в себе значения для разных заголовков
        // Например: в заголовке "tx_queue rx_queue", а в значении с номером, соответствующем local_address,
        // "00000:00000". Для того, что бы учесть этот случай, вводятся переменные header_index и value_index. При
        // встрече "сдвоенного значения" header_index увеличивается 2 раза, value_index - один.

        while (header_index < header.size()) {

            if (header[header_index] == "tx_queue" || header[header_index] == "tr") {
                auto split_val = split(values[value_index], ':');

                socket[header[header_index]] = split_val[0];
                socket[header[header_index+1]] = split_val[1];
                header_index++;
            } else {
                socket[header[header_index]] = values[value_index];
            }

            header_index++;
            value_index++;

        }

        sockets_list.push_back(socket);
    }

    return sockets_list;
}