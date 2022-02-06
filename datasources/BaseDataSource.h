//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_BASEDATASOURCE_H
#define LFNP_BASEDATASOURCE_H

#include <optional>

#include <QtCore/QString>
#include <QThread>

#include "../types/SocketInfo.h"
#include "../types/InSystemTimeInfo.h"

#include "../utils/sockets.h"
#include "../utils/utils.h"
#include "../types/DropsInfo.h"
#include "../types/enums/CPUDistributionSource.h"
#include "../types/enums/MeasureType.h"
#include "../types/BandwidthResult.h"

typedef QMap<QString, int> ProtocolStats;

class BaseDataSource {

public:

    //TODO: Описание в Linux, объединение названий общих для FreeBSD и Linux значений
    virtual std::optional<ProtocolStats> getProtocolStats(const QString &protocol)=0;

    virtual QVector<SocketInfo> getSockets(QString protocol)=0;

    //TODO: Проверить hardware timestamps
    //TODO: В FreeBSD значение Total огромное. Почему?
    //TODO: CPU affinity для timestamp'ов
    std::optional<QVector<ReceiveTimestamp>> recvTimestamps(
            const QString &protocol, unsigned int port, unsigned int packets_count);

    std::optional<QVector<SendTimestamp>> sendTimestamps(
            const QString &protocol,
            const QString &addr,
            unsigned int port,
            unsigned int packets_count,
            const QString& measure_type,
            unsigned int delay,

            const QString &data_filename,
            quint64 data_size,
            bool zero_copy);

    //TODO: как реализовать многопоточность для udp и udplite?
    std::optional<BandwidthResult> recvBandwidth(const QString &protocol, unsigned int port,
                                                 quint64 threads_count, bool cpu_affinity);

    std::optional<BandwidthResult> sendBandwidth(const QString &protocol,
                       const QString &addr,
                       unsigned int port,
                       quint64 duration,
                       const QString &data_filename,
                       quint64 data_size,
                       bool zero_copy,
                       quint64 threads_count,
                       bool cpu_affinity);

    virtual std::optional<QMap<int, int>> getCPUDistribution(CPUDistributionSource source)=0;
    virtual std::optional<QMap<QString, DropsInfo>> getDevsDropsInfo()=0;
    virtual QVector<QPair<QString, DropsInfo>> getDropsInfo()=0;

    virtual void setRecvSockOpt(Socket &sock)=0;
    virtual void
    processRecvTimestamp(msghdr &msg, ReceiveTimestamp &res, timespec &after_recv_time, const QString &protocol)=0;

    virtual void setSendSockOpt(Socket &sock, MeasureType measure_type)=0;
    virtual void
    processSendTimestamp(Socket &sock, SendTimestamp &res, TimeRange &timestamps)=0;

    virtual QStringList getSupportedSocketsListProtocols()=0;
    virtual QStringList getSupportedStatsProtocols()=0;

    int getOneStat(const QString &protocol, const QString &stat_name);
    std::optional<SocketInfo> getOneSocket(const QString &protocol, const QString &local_address, unsigned int local_port,
                                           const QString &foreign_address, unsigned int foreign_port);
};


#endif //LFNP_BASEDATASOURCE_H
