//
// Created by shenk on 22.12.2021.
//

#include "printers.h"

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
    std::cout << std::left << std::setw(ADDRESS_WIDTH) << (si.local_address + ":"  + QString::number(si.local_port)).toStdString();
    std::cout << std::left << std::setw(ADDRESS_WIDTH) << (si.foreign_address + ":"  + QString::number(si.foreign_port)).toStdString();
    std::cout << std::left << std::setw(QUEUE_WIDTH) << si.rx_queue_size;
    std::cout << std::left << std::setw(QUEUE_WIDTH) << si.tx_queue_size;

#ifdef __linux__
    std::cout << std::left << std::setw(OTHER_WIDTH) << si.ref;
    std::cout << std::left << std::setw(OTHER_WIDTH) << si.drops;
#endif

    std::cout << std::endl;
}

void printSocketsInfoList(const QVector<SocketInfo>& sockets_info_list) {

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

void printBandwidthResult(std::optional<BandwidthResult> o_res) {

    if (o_res) {
        std::cout << "Transfered, GBytes: " << (double)o_res->bytes_count/1000000000L << std::endl;
        std::cout << "Bandwidth, GBites/s: " << (double)o_res->bytes_count/o_res->duration/1000000000L*8 << std::endl;
    } else {
        std::cout << "Can't measure bandwidth" << std::endl;
    }

}

void _printProfilerNode(const FuncProfilerTreeNode *node, int indent = 0) {

    std::cout << QString("\t").repeated(indent).toStdString()
              << node->getFuncName().toStdString() << " " << node->getDuration() << std::endl;

    for (const auto child: node->getChildren()) {
        _printProfilerNode(child, indent+1);
    }

}

void printProfilerData(ProfilerData profiler_data) {
    for (const auto cpu: profiler_data.keys()) {
        std::cout << "CPU: " << cpu << std::endl;
        for (const auto pid: profiler_data[cpu].keys()) {
            std::cout << "PID: " << pid << std::endl;
            for (const auto *root: profiler_data[cpu][pid]) {
                _printProfilerNode(root);
            }
        }
    }
}
