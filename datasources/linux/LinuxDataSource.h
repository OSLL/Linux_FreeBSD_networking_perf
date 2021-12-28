//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_LINUXDATASOURCE_H
#define LFNP_LINUXDATASOURCE_H

#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <linux/net_tstamp.h>
#include <linux/errqueue.h>
#include <unistd.h>

#include <QMap>

#include "../../utils/utils.h"
#include "parsers/parsers.h"

#include "../BaseDataSource.h"


class LinuxDataSource: public BaseDataSource {

private:

    static std::map<std::string, std::string> protocol_sockets_files;
    static QMap<QString, std::tuple<int, int, int>> protocol_socket_args;

public:

    int getTcpTotalRecv() override;

    std::vector<SocketInfo> getSockets(std::string protocol) override;

    std::optional<InSystemTimeRXInfo> getInSystemTimeRX(
            const QString& protocol, unsigned int port, unsigned int packets_count) override;

    std::optional<InSystemTimeTXInfo> sendTimestamp(
            const QString &protocol,
            const QString &addr,
            unsigned int port,
            unsigned int packets_count,
            const QString& measure_type) override;
};


#endif //LFNP_LINUXDATASOURCE_H
