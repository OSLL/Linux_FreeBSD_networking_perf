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
#include <QThread>

#include "../../utils/utils.h"
#include "../../utils/sockets.h"
#include "parsers/parsers.h"

#include "../BaseDataSource.h"


class LinuxDataSource: public BaseDataSource {

private:

    static std::map<std::string, std::string> protocol_sockets_files;
    static QMap<QString, std::tuple<std::optional<QString>, std::optional<QString>>> protocol_stats_names;

    std::optional<QMap<QString, int>> _getProtocolStats(const QString &protocol);
    std::optional<QMap<QString, int>> _getProtocolV6Stats(const QString &protocol);

public:

    std::optional<QMap<QString, int>> getProtocolStats(const QString &protocol) override;

    std::vector<SocketInfo> getSockets(std::string protocol) override;

    std::optional<InSystemTimeInfo> recvTimestamp(
            const QString& protocol, unsigned int port, unsigned int packets_count) override;

    std::optional<InSystemTimeInfo> sendTimestamp(
            const QString &protocol,
            const QString &addr,
            unsigned int port,
            unsigned int packets_count,
            const QString& measure_type,
            unsigned int delay) override;

    std::optional<CpusSoftnetData> getSoftnetData();

    std::optional<QVector<int>> getCPUDistribution() override;
};


#endif //LFNP_LINUXDATASOURCE_H
