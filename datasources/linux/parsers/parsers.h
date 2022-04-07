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

#include <QMap>
#include <QFile>
#include <QStack>

#include "../../../utils/utils.h"
#include "../../../types/FuncProfiler.h"

typedef QMap<QString, QMap<QString, int>> ProtocolsStats;
typedef QVector<QMap<QString, QString>> SocketsList;
typedef QVector<QVector<int>> CpusSoftnetData;
typedef QMap<QString, QMap<int, int>> CpusSoftirqData;

std::optional<ProtocolsStats> parseProtocolsStatsFile(const QString& filename);
std::optional<ProtocolsStats> parseProtocolsV6StatsFile(const QString& filename);

std::optional<SocketsList> parseProtocolSocketsListFile(const QString& filename);
std::optional<CpusSoftnetData> parseSoftnetDataFile(const QString& filename);
std::optional<CpusSoftirqData> parseSoftirqFile(const QString& filename);

#endif //LFNP_PARSERS_H
