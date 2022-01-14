//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_BASEDATASOURCE_H
#define LFNP_BASEDATASOURCE_H

#include <vector>
#include <optional>
#include <QtCore/QString>

#include "../types/SocketInfo.h"
#include "../types/InSystemTimeInfo.h"

class BaseDataSource {

public:

    //TODO: Описание в Linux, объединение названий общих для FreeBSD и Linux значений
    virtual std::optional<QMap<QString, int>> getProtocolStats(const QString &protocol)=0;

    virtual std::vector<SocketInfo> getSockets(std::string protocol)=0;

    virtual std::optional<InSystemTimeInfo> recvTimestamp(
            const QString &protocol, unsigned int port, unsigned int packets_count)=0;

    virtual std::optional<InSystemTimeInfo> sendTimestamp(
            const QString &protocol,
            const QString &addr,
            unsigned int port,
            unsigned int packets_count,
            const QString& measure_type,
            unsigned int delay)=0;

    virtual std::optional<QVector<int>> getCPUDistribution()=0;
};


#endif //LFNP_BASEDATASOURCE_H
