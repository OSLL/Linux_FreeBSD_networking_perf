//
// Created by shenk on 21.12.2021.
//

#ifndef LFNP_PARSERS_H
#define LFNP_PARSERS_H

#include <map>
#include <vector>
#include <optional>
#include <string>
#include <iostream>
#include <fstream>

#include "../../../utils/utils.h"

typedef std::map<std::string, std::map<std::string, int>> ProtocolsStats;
typedef std::vector<std::map<std::string, std::string>> SocketsList;

std::optional<ProtocolsStats> parseProtocolsStatsFile(const std::string& filename);

std::optional<SocketsList> parseProtocolSocketsListFile(const std::string& filename);

#endif //LFNP_PARSERS_H
