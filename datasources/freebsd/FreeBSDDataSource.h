//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_FREEBSDDATASOURCE_H
#define LFNP_FREEBSDDATASOURCE_H

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netinet/in_pcb.h>
#include "netinet/tcp_var.h"

#include "../BaseDataSource.h"
#include "../../utils/sockets.h"

#include <sys/sysctl.h>

#include <iostream>
#include <map>

#include "../../utils/utils.h"
class FreeBSDDataSource: public BaseDataSource {

private:

    static std::map<std::string, std::string> protocol_sockets_sysctl_names;

public:

    int getTcpTotalRecv() override;

    std::vector<SocketInfo> getSockets(std::string protocol) override;

    std::optional<InSystemTimeRXInfo> getInSystemTimeRX(
            const QString &protocol, unsigned int port, unsigned int packets_count) override;

    std::optional<InSystemTimeTXInfo> sendTimestamp(
            const QString &protocol,
            const QString &ip_addr,
            unsigned int port,
            unsigned int packets_count,
            const QString& measure_type) override;
};


#endif //LFNP_FREEBSDDATASOURCE_H
