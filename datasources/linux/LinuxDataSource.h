//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_LINUXDATASOURCE_H
#define LFNP_LINUXDATASOURCE_H

#include <netinet/in.h>
#include <arpa/inet.h>

#include "../../utils/utils.h"
#include "parsers/parsers.h"

#include "../BaseDataSource.h"


class LinuxDataSource: public BaseDataSource {

private:

    static std::map<std::string, std::string> protocol_sockets_files;

public:

    int getTcpTotalRecv() override;

    std::vector<SocketInfo> getSockets(std::string protocol) override;

    InSystemTimeInfo getInSystemTime() override;


};


#endif //LFNP_LINUXDATASOURCE_H
