//
// Created by shenk on 21.12.2021.
//

#ifndef LFNP_PARSERS_H
#define LFNP_PARSERS_H

#include <vector>
#include <optional>
#include <string>
#include <iostream>
#include <fstream>

#include <QtCore/QMap>
#include <QtCore/QFile>

#include "../../../utils/utils.h"

typedef QMap<QString, QMap<QString, int>> ProtocolsStats;
typedef std::vector<std::map<std::string, std::string>> SocketsList;
typedef QVector<QVector<int>> CpusSoftnetData;
typedef QMap<QString, QMap<int, int>> CpusSoftirqData;

std::optional<ProtocolsStats> parseProtocolsStatsFile(const QString& filename);
std::optional<ProtocolsStats> parseProtocolsV6StatsFile(const QString& filename);

std::optional<SocketsList> parseProtocolSocketsListFile(const std::string& filename);
std::optional<CpusSoftnetData> parseSoftnetDataFile(const QString& filename);
std::optional<CpusSoftirqData> parseSoftirqFile(const QString& filename);

#endif //LFNP_PARSERS_H
