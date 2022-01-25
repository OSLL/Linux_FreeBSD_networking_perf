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

    std::optional<CpusSoftnetData> getSoftnetData();

    std::optional<QMap<int, int>> getCPUDistribution(CPUDistributionSource source) override;

    std::optional<QMap<QString, DropsInfo>> getDevsDropsInfo() override;

    QVector<QPair<QString, DropsInfo>> getDropsInfo() override;

    void setRecvSockOpt(Socket &sock) override;
    void processRecvTimestamp(msghdr &msg, InSystemTimeInfo &res, timespec &after_recv_time,
                              const QString &protocol) override;

    void setSendSockOpt(Socket &sock, const QString &measure_type) override;
    void
    processSendTimestamp(Socket &sock, InSystemTimeInfo &res, TimeRange &timestamps) override;
};


#endif //LFNP_LINUXDATASOURCE_H
