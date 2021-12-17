//
// Created by shenk on 17.12.2021.
//

#include "LinuxDataSource.h"
#include "../../utils/utils.h"

int LinuxDataSource::getTcpTotalRecv() {

    auto protocols_stats = this->parseProtocolsStatsFile("/proc/net/snmp");
    if (protocols_stats) {
        return protocols_stats.value()["Tcp"]["InSegs"];
    }
    return 0;

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

            // Remove empty string because of first space
            header.erase(header.begin());

            // Next line is data
            is_header = false;

        } else {

            auto string_data = split(v[1], ' ');

            // Remove empty string because of first space
            string_data.erase(string_data.begin());

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

            // Next line is header
            is_header = true;
        }

    }

    return stats;

}
