//
// Created by shenk on 22.12.2021.
//

#include <iomanip>
#include <optional>
#include "printers.h"
#include "../utils/utils.h"

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

void printTimestamp(const timespec &ts, bool in_ms) {
    std::cout << ts.tv_sec << " sec ";
    if (in_ms) {
        std::cout << ts.tv_nsec/1000 << " µs" << std::endl;
    } else {
        std::cout << ts.tv_nsec << " ns" << std::endl;
    }
}

void printInSystemTimeInfo(std::optional<InSystemTimeInfo> o_time_info, bool in_ms) {

    if (o_time_info) {

        if (!is_timespec_empty(o_time_info->hardware_time)) {
            std::cout << "Hardware: ";
            printTimestamp(o_time_info->hardware_time, in_ms);
        }

        if (!is_timespec_empty(o_time_info->software_time)) {
            std::cout << "Software: ";
            printTimestamp(o_time_info->software_time, in_ms);
        }

        if (!is_timespec_empty(o_time_info->total_time)) {
            std::cout << "Total: ";
            printTimestamp(o_time_info->total_time, in_ms);
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
