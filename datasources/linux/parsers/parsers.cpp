//
// Created by shenk on 21.12.2021.
//

#include "parsers.h"

std::optional<ProtocolsStats> parseProtocolsStatsFile(const QString& filename) {

    ProtocolsStats stats;

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
        return std::nullopt;
    }

    bool is_header = true;

    QString protocol_name;
    QStringList header;

    do {

        QString line = file.readLine();

        auto v = line.split(':');

        protocol_name = v[0];

        if (is_header) {
            // При этом в названиях заголовка остаются символы переноса, их нужно убрать
            header = v[1].split(' ', Qt::SkipEmptyParts);
            // Убираем переносы
            for (auto &h: header) {
                h = h.trimmed();
            }

            // На следующей строке - информация
            is_header = false;

        } else {

            auto string_data = v[1].split(' ', Qt::SkipEmptyParts);

            std::vector<int> data;
            data.reserve(string_data.size());

            for (const auto& s: string_data) {
                data.push_back(s.toInt());
            }

            QMap<QString, int> protocol_stats;

            for (int i=0; i<header.size(); i++) {
                protocol_stats[header[i]] = data[i];
            }

            stats[protocol_name] = protocol_stats;

            // На следующей строке - заголовок
            is_header = true;
        }

    } while (!file.atEnd());

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
        // Например: в заголовке "tx_queue rx_queue", а в значении с номером, соответствующем tx_queue,
        // "00000:00000". Для того, что бы учесть этот случай, вводятся переменные header_index и value_index. При
        // встрече "сдвоенного значения" header_index увеличивается 2 раза, value_index - один.
        // Но "сдвоенное значение" - это так же local_address, в соответсвующем значении которого - и адрес и порт, но
        // в заголовке - только local_address
        // Таким образом, как "сдвоенное" необходимо рассматривать только колонки tx_queue и tr

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

        // В файле /proc/net/tcp не все значения имеют заголовки, поэтому остались необработанные значения. Среди них -
        // число ссылок, которые используются в программе
        if (value_index < values.size()) {
            socket["ref"] = values[value_index];
        }

        sockets_list.push_back(socket);
    }

    return sockets_list;
}
