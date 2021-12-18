//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_LINUXDATASOURCE_H
#define LFNP_LINUXDATASOURCE_H

#include <fstream>
#include <iostream>
#include <map>
#include <optional>

#include "../BaseDataSource.h"

typedef std::map<std::string, std::map<std::string, int>> ProtocolsStats;

class LinuxDataSource: public BaseDataSource {

private:

    std::optional<ProtocolsStats> parseProtocolsStatsFile(std::string filename);

public:

    int getTcpTotalRecv() override;


};


#endif //LFNP_LINUXDATASOURCE_H
