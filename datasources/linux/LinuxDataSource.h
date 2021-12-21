//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_LINUXDATASOURCE_H
#define LFNP_LINUXDATASOURCE_H

#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <vector>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "../../utils/utils.h"

#include "../BaseDataSource.h"

typedef std::map<std::string, std::map<std::string, int>> ProtocolsStats;
typedef std::vector<std::map<std::string, std::string>> SocketsList;

class LinuxDataSource: public BaseDataSource {

private:

    std::optional<ProtocolsStats> parseProtocolsStatsFile(std::string filename);
    std::optional<SocketsList> parseProtocolSocketsListFile(std::string filename);

    static std::map<std::string, std::string> protocol_sockets_files;

public:

    int getTcpTotalRecv() override;

    std::vector<SocketInfo> getSockets(std::string protocol) override;


};


#endif //LFNP_LINUXDATASOURCE_H
