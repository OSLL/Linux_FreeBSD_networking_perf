//
// Created by shenk on 22.12.2021.
//

#ifndef LFNP_PRINTERS_H
#define LFNP_PRINTERS_H

#include <vector>
#include <iostream>

#include "../types/SocketInfo.h"
#include "../types/InSystemTimeInfo.h"

void printSocketsInfoList(const std::vector<SocketInfo>& sockets_info_list);

void printInSystemTimeInfo(std::optional<InSystemTimeInfo> o_time_info);

#endif //LFNP_PRINTERS_H
