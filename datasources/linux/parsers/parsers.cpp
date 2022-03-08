//
// Created by shenk on 21.12.2021.
//

#include "parsers.h"
#include "../../../types/FuncProfiler.h"
#include "../../../types/TimeRange.h"

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

std::optional <SocketsList> parseProtocolSocketsListFile(const QString& filename) {

    SocketsList sockets_list;

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
        return std::nullopt;
    }

    QString line = file.readLine().trimmed();
    auto header = line.split(' ', Qt::SkipEmptyParts);

    while (!file.atEnd()) {
        line = file.readLine().trimmed();
        auto values = line.split(' ', Qt::SkipEmptyParts);

        QMap<QString, QString> socket;

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
                auto split_val = values[value_index].split(':', Qt::SkipEmptyParts);

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

std::optional<ProtocolsStats> parseProtocolsV6StatsFile(const QString &filename) {
    ProtocolsStats stats;

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
        return std::nullopt;
    }

    do {
        QString line = file.readLine();
        auto v = line.split(' ', Qt::SkipEmptyParts);

        auto protocol_stat_name = v[0].split('6');

        QString protocol = protocol_stat_name[0].toLower() + '6';
        QString stat_name = protocol_stat_name[1];
        int stat_val = v[1].toInt();

        if (stats.contains(protocol)) {
            stats[protocol][stat_name] = stat_val;
        } else {
            stats[protocol] = {
                    {stat_name, stat_val}
            };
        }

    } while (!file.atEnd());

    return stats;
}

std::optional<CpusSoftnetData> parseSoftnetDataFile(const QString &filename) {

    CpusSoftnetData cpus_sd;
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
        return std::nullopt;
    }

    do {

        QString line = file.readLine();
        auto current_sd = line.split(' ');

        cpus_sd.push_back(QVector<int>());
        for (const auto &sd_field: current_sd) {
            cpus_sd.last().push_back(sd_field.toInt(nullptr, 16));
        }

    } while (!file.atEnd());

    return cpus_sd;
}

std::optional<CpusSoftirqData> parseSoftirqFile(const QString &filename) {
    CpusSoftirqData cpus_sirq;
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
        return std::nullopt;
    }

    QString header = file.readLine();
    QStringList cpu_names = header.trimmed().split(' ', Qt::SkipEmptyParts);

    QVector<int> cpu_nums;
    for (auto &name: cpu_names) {
        cpu_nums.push_back(name.remove(0, 3).toInt());
    }

    do {

        QString line = file.readLine();
        QStringList name_vals = line.split(':');
        QString sirq_name = name_vals[0].trimmed();

        QString vals = name_vals[1];
        QStringList s_sirq_count = vals.split(' ', Qt::SkipEmptyParts);

        QMap<int, int> sirq_count;

        for (int i=0; i<cpu_nums.size(); i++) {
            sirq_count[cpu_nums[i]] = s_sirq_count[i].toInt();
        }

        cpus_sirq[sirq_name] = sirq_count;

    } while (!file.atEnd());

    return cpus_sirq;
}

void _parseProfilerData(QFile &file, FuncProfilerTreeNode *parent, quint64 enter_time) {

    bool is_return = false;
    while (!(is_return || file.atEnd()) ) {

        QString line = file.readLine();
        QStringList profiler_tokens = line.split(' ');

        if (profiler_tokens[0] == "enter") {

            auto child = new FuncProfilerTreeNode(
                    profiler_tokens[1], profiler_tokens[3].toInt(), parent);
            _parseProfilerData(file, child, profiler_tokens[2].toULongLong());
            parent->addChildrenAsTime(child);

        } else if (profiler_tokens[0] == "return") {
            is_return = true;

            if (profiler_tokens[1] == parent->getFuncName()) {
                quint32 cpu_index = profiler_tokens[3].toUInt();
                parent->addRange(TimeRangeNS(enter_time, profiler_tokens[2].toULongLong()));

                if (cpu_index != parent->getCPUIndex()) {
                    std::cout << "Stack warning: different enter and return CPU" << std::endl;
                }
            } else {
                std::cout << "Stack error: different enter and return names" << std::endl;
            }

        }

    }
}

std::optional<FuncProfilerTreeNode*> parseProfilerData(const QString &filename) {

    QStack<QStringList> stack;
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
        return std::nullopt;
    }

    auto *root = new FuncProfilerTreeNode("root", 0, NULL);

    do {

        QString line = file.readLine();
        QStringList profiler_tokens = line.split(' ');

        if (profiler_tokens[0] == "enter") {

            auto child = new FuncProfilerTreeNode(
                    profiler_tokens[1], profiler_tokens[3].toInt(), root);
            _parseProfilerData(file, child, profiler_tokens[2].toULongLong());
            root->addChildrenAsTime(child);

        } else if (profiler_tokens[0] == "return") {
            std::cout << "Warning: return at top level" << std::endl;
        }

    } while (!file.atEnd());

    return root;
}
