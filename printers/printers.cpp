//
// Created by shenk on 22.12.2021.
//

#include <iomanip>
#include <optional>
#include "printers.h"
#include "../utils/utils.h"

#define ADDRESS_WIDTH 25
#define QUEUE_WIDTH 10

void printHeader() {
    std::cout << std::left << std::setw(ADDRESS_WIDTH) << "Local address";
    std::cout << std::left << std::setw(ADDRESS_WIDTH) << "Foreign address";
    std::cout << std::left << std::setw(QUEUE_WIDTH) << "RX Queue";
    std::cout << std::left << std::setw(QUEUE_WIDTH) << "TX Queue";
    std::cout << std::endl;
}

void printSocketInfo(const SocketInfo &si) {
    std::cout << std::left << std::setw(ADDRESS_WIDTH) << si.local_address + ":"  + std::to_string(si.local_port);
    std::cout << std::left << std::setw(ADDRESS_WIDTH) << si.foreign_address + ":"  + std::to_string(si.foreign_port);
    std::cout << std::left << std::setw(QUEUE_WIDTH) << si.rx_queue_size;
    std::cout << std::left << std::setw(QUEUE_WIDTH) << si.tx_queue_size;
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
