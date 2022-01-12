//
// Created by shenk on 22.12.2021.
//

#ifndef LFNP_PRINTERS_H
#define LFNP_PRINTERS_H

#include <vector>
#include <iostream>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include "../types/SocketInfo.h"
#include "../types/InSystemTimeInfo.h"

void printSocketsInfoList(const std::vector<SocketInfo>& sockets_info_list);

void printInSystemTimeInfo(std::optional<InSystemTimeInfo> o_time_info, bool in_ms);

void printIperf3Data(const std::optional<QJsonArray>& o_json_data);

void printProtocolStats(std::optional<QMap<QString, int>> o_protocol_stats);

void printSoftnetData(std::optional<QVector<QVector<int>>> o_cpus_sd);

#endif //LFNP_PRINTERS_H
