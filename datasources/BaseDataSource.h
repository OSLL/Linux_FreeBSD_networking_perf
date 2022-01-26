//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_BASEDATASOURCE_H
#define LFNP_BASEDATASOURCE_H

#include <vector>
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

class BaseDataSource {

public:

    //TODO: Описание в Linux, объединение названий общих для FreeBSD и Linux значений
    virtual std::optional<QMap<QString, int>> getProtocolStats(const QString &protocol)=0;

    virtual std::vector<SocketInfo> getSockets(std::string protocol)=0;

    //TODO: Проверить hardware_recv timestamps
    //TODO: В FreeBSD значение Total огромное. Почему?
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

    virtual std::optional<QMap<int, int>> getCPUDistribution(CPUDistributionSource source)=0;
    virtual std::optional<QMap<QString, DropsInfo>> getDevsDropsInfo()=0;
    virtual QVector<QPair<QString, DropsInfo>> getDropsInfo()=0;

    virtual void setRecvSockOpt(Socket &sock)=0;
    virtual void
    processRecvTimestamp(msghdr &msg, ReceiveTimestamp &res, timespec &after_recv_time, const QString &protocol)=0;

    virtual void setSendSockOpt(Socket &sock, const MeasureType measure_type)=0;
    virtual void
    processSendTimestamp(Socket &sock, SendTimestamp &res, TimeRange &timestamps)=0;
};


#endif //LFNP_BASEDATASOURCE_H
