//
// Created by shenk on 22.12.2021.
//

#ifndef LFNP_PRINTERS_H
#define LFNP_PRINTERS_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <optional>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include "../../types/SocketInfo.h"
#include "../../types/InSystemTimeInfo.h"
#include "../../types/DropsInfo.h"
#include "../../utils/utils.h"
#include "../../types/BandwidthResult.h"
#include "../../types/FuncProfiler.h"

void printSocketsInfoList(const QVector<SocketInfo>& sockets_info_list);

void printTimestamp(const quint64 &ts, bool in_ms);

void printProtocolStats(std::optional<QMap<QString, int>> o_protocol_stats);

void printSoftnetData(std::optional<QVector<QVector<int>>> o_cpus_sd);

void printCPUDistribution(std::optional<QMap<int, int>> o_cpus_dist);

void printDropsInfo(QVector<QPair<QString, DropsInfo>> drops_info);

void printBandwidthResult(std::optional<BandwidthResult> o_res);

void printProfilerData(std::optional<FuncProfilerTreeNode*>, int indent=-1);

template <typename T>
void printTimestampsAverage(std::optional<QVector<T>> &o_time_info, bool in_ms) {

    if (o_time_info) {
        std::cout << "Size: " << o_time_info->size() << std::endl;

        //TODO: как переписать по нормальному?
        auto avg_software = get_average_o<T>(*o_time_info, [](T &timestamp) { return timestamp.getSoftwareTime(); });
        auto avg_hardware = get_average_o<T>(*o_time_info, [](T &timestamp) { return timestamp.getHardwareTime(); });
        auto avg_in_call = get_average<T>(*o_time_info, [](T &timestamp) { return timestamp.getInCallTime(); });
        auto avg_total = get_average<T>(*o_time_info, [](T &timestamp) { return timestamp.getTotalTime(); });

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
#endif //LFNP_PRINTERS_H
