//
// Created by shenk on 22.12.2021.
//

#include <iomanip>
#include <optional>
#include "printers.h"
#include "../../utils/utils.h"

#define ADDRESS_WIDTH 25
#define QUEUE_WIDTH 10
#define OTHER_WIDTH 5

void printHeader() {
    std::cout << std::left << std::setw(ADDRESS_WIDTH) << "Local address";
    std::cout << std::left << std::setw(ADDRESS_WIDTH) << "Foreign address";
    std::cout << std::left << std::setw(QUEUE_WIDTH) << "RX Queue";
    std::cout << std::left << std::setw(QUEUE_WIDTH) << "TX Queue";

#ifdef __linux__
    std::cout << std::left << std::setw(OTHER_WIDTH) << "Refs";
    std::cout << std::left << std::setw(OTHER_WIDTH) << "Drops";
#endif

    std::cout << std::endl;
}

void printSocketInfo(const SocketInfo &si) {
    std::cout << std::left << std::setw(ADDRESS_WIDTH) << si.local_address + ":"  + std::to_string(si.local_port);
    std::cout << std::left << std::setw(ADDRESS_WIDTH) << si.foreign_address + ":"  + std::to_string(si.foreign_port);
    std::cout << std::left << std::setw(QUEUE_WIDTH) << si.rx_queue_size;
    std::cout << std::left << std::setw(QUEUE_WIDTH) << si.tx_queue_size;

#ifdef __linux__
    std::cout << std::left << std::setw(OTHER_WIDTH) << si.ref;
    std::cout << std::left << std::setw(OTHER_WIDTH) << si.drops;
#endif

    std::cout << std::endl;
}

void printSocketsInfoList(const std::vector<SocketInfo>& sockets_info_list) {

    printHeader();
    for (const auto &si: sockets_info_list) {
        printSocketInfo(si);
    }

}

void printTimestamp(const quint64 &ts, bool in_ms) {
    if (in_ms) {
        std::cout << ts/1000 << " us" << std::endl;
    } else {
        std::cout << ts << " ns" << std::endl;
    }
}

void printInSystemTimeInfo(std::optional<InSystemTimeInfo> o_time_info, bool in_ms) {

    if (o_time_info) {

        auto avg_software = o_time_info->getAverageSoftware();
        auto avg_hardware = o_time_info->getAverageHardware();
        auto avg_in_call = o_time_info->getAverageInCall();
        auto avg_total = o_time_info->getAverageTotal();

        if (avg_hardware) {
            std::cout << "Hardware: ";
            printTimestamp(avg_hardware, in_ms);
        }

        if (avg_software) {
            std::cout << "Software: ";
            printTimestamp(avg_software, in_ms);
        }

        if (avg_in_call) {
            std::cout << "In call: ";
            printTimestamp(avg_in_call, in_ms);
        }

        if (avg_total) {
            std::cout << "Total: ";
            printTimestamp(avg_total, in_ms);
        }

    } else {
        std::cout << "Can't measure timings" << std::endl;
    }

}

#define INTERVAL_COLUMN_WIDTH 20

void printIperf3Data(const std::optional<QJsonArray> &o_json_data) {

    if (o_json_data) {

        for (int test_index=0; test_index<o_json_data->size(); test_index++) {
            auto test = o_json_data.value()[test_index];
            auto intervals = test["intervals"].toArray();

            if (intervals.empty()) continue;

            std::cout << "Test #" << test_index << std::endl;
            std::cout << std::left << std::setw(INTERVAL_COLUMN_WIDTH) << "From";
            std::cout << std::left << std::setw(INTERVAL_COLUMN_WIDTH) << "To";
            std::cout << std::left << std::setw(INTERVAL_COLUMN_WIDTH) << "Bits/s" << std::endl;

            std::cout << std::fixed << std::left;

            for (const auto& interval: test["intervals"].toArray()) {
                auto interval_sum = interval.toObject()["sum"].toObject();
                std::cout << std::setw(INTERVAL_COLUMN_WIDTH) << std::setprecision(4) << interval_sum["start"].toDouble();
                std::cout << std::setw(INTERVAL_COLUMN_WIDTH) << std::setprecision(4) << interval_sum["end"].toDouble();
                std::cout << std::setw(INTERVAL_COLUMN_WIDTH) << std::setprecision(0) << interval_sum["bits_per_second"].toDouble() << std::endl;
            }
        }
    } else {
        std::cout << "Can't get iperf3 data" << std::endl;
    }

}

void printProtocolStats(std::optional<QMap<QString, int>> o_protocol_stats) {

    if (!o_protocol_stats) {
        std::cout << "Can't get protocol stats" << std::endl;
    } else {

        auto protocol_stats = o_protocol_stats.value();

        for (auto it = protocol_stats.begin(); it != protocol_stats.end(); it++) {
            std::cout << it.key().toStdString() << ": " << it.value() << std::endl;
        }
    }
}

//TODO: поддержка старых версий файла
#define SOFTNET_COLUMN_WIDTH 20
void printSoftnetData(std::optional<QVector<QVector<int>>> o_cpus_sd) {

    if (o_cpus_sd) {
        auto cpus_sd = o_cpus_sd.value();

        std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << "Processed";
        std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << "Dropped";
        std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << "Time squeeze";
        std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << "Received RPS";
        std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << "Flow Limit count";
        std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << "Backlog len";
        std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << "CPU Index" << std::endl;

        for (const auto &current_sd: cpus_sd) {

            std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << current_sd[0];
            std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << current_sd[1];
            std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << current_sd[2];
            std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << current_sd[9];
            std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << current_sd[10];
            std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << current_sd[11];
            std::cout << std::left << std::setw(SOFTNET_COLUMN_WIDTH) << current_sd[12] << std::endl;

        }

    } else {
        std::cout << "Can't get softnet data" << std::endl;
    }

}

void printCPUDistribution(std::optional<QMap<int, int>> o_cpus_dist) {

    if (o_cpus_dist) {
        for (auto it = o_cpus_dist.value().begin(); it != o_cpus_dist.value().end(); it++) {
            std::cout << "CPU" << it.key() << ": " << it.value() << std::endl;
        }
        std::cout << std::endl;
    } else {
        std::cout << "Can't get CPU distribution" << std::endl;
    }

}

#define DROPS_COLUMN_WIDTH 20

void printDropsInfo(QVector<QPair<QString, DropsInfo>> drops_info) {

    std::cout << std::left << std::setw(DROPS_COLUMN_WIDTH) << "Source";
    std::cout << std::left << std::setw(DROPS_COLUMN_WIDTH) << "RX Drops";
    std::cout << std::left << std::setw(DROPS_COLUMN_WIDTH) << "TX Drops";
    std::cout << std::endl;

    for (const auto &it: drops_info) {
        std::cout << std::left << std::setw(DROPS_COLUMN_WIDTH) << it.first.toStdString();
        if (it.second.is_common) {
            std::cout << std::right << std::setw(DROPS_COLUMN_WIDTH/2) << it.second.common_drops;
        } else {
            std::cout << std::left << std::setw(DROPS_COLUMN_WIDTH) << it.second.rx_drops;
            std::cout << std::left << std::setw(DROPS_COLUMN_WIDTH) << it.second.tx_drops;
        }
        std::cout << std::endl;
    }
}
