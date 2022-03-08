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
#include "../../types/FuncProfiler.h"


class LinuxDataSource: public BaseDataSource {

private:

    static QMap<QString, QString> protocol_sockets_files;
    static QMap<QString, std::tuple<std::optional<QString>, std::optional<QString>>> protocol_stats_names;

    std::optional<QMap<QString, int>> _getProtocolStats(const QString &protocol);
    std::optional<QMap<QString, int>> _getProtocolV6Stats(const QString &protocol);

public:

    std::optional<ProtocolStats> getProtocolStats(const QString &protocol) override;

    QVector<SocketInfo> getSockets(QString protocol) override;

    std::optional<CpusSoftnetData> getSoftnetData();

    std::optional<QMap<int, int>> getCPUDistribution(CPUDistributionSource source) override;

    std::optional<QMap<QString, DropsInfo>> getDevsDropsInfo() override;

    QVector<QPair<QString, DropsInfo>> getDropsInfo() override;

    void setRecvSockOpt(Socket &sock) override;
    void processRecvTimestamp(msghdr &msg, ReceiveTimestamp &res, timespec &after_recv_time,
                              const QString &protocol) override;

    void setSendSockOpt(Socket &sock, MeasureType measure_type) override;
    void
    processSendTimestamp(Socket &sock, SendTimestamp &res, TimeRange &timestamps) override;

    QStringList getSupportedSocketsListProtocols() override;
    QStringList getSupportedStatsProtocols() override;
    std::optional<FuncProfilerTreeNode*> getProfilerData() override;
};


#endif //LFNP_LINUXDATASOURCE_H
