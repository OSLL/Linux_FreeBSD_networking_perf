//
// Created by shenk on 21.12.2021.
//

#include "parsers.h"

std::optional<ProtocolsStats> parseProtocolsStatsFile(const std::string& filename) {

    ProtocolsStats stats;

    std::ifstream file(filename);

    if (!file) {
        std::cout << "Can't open " << filename << std::endl;
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

std::optional <SocketsList> parseProtocolSocketsListFile(const std::string& filename) {

    std::vector<std::map<std::string, std::string>> sockets_list;

    std::ifstream file(filename);

    if (!file) {
        std::cout << "Can't open " << filename << std::endl;
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
