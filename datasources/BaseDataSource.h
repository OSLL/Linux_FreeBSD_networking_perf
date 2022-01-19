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


class BaseDataSource {

public:

    //TODO: Описание в Linux, объединение названий общих для FreeBSD и Linux значений
    virtual std::optional<QMap<QString, int>> getProtocolStats(const QString &protocol)=0;

    virtual std::vector<SocketInfo> getSockets(std::string protocol)=0;

    //TODO: Проверить hardware timestamps
    //TODO: В FreeBSD значение Total огромное. Почему?
    //TODO: Добавить поддерку отправки данных
    virtual std::optional<InSystemTimeInfo> recvTimestamp(
            const QString &protocol, unsigned int port, unsigned int packets_count);

    virtual std::optional<InSystemTimeInfo> sendTimestamp(
            const QString &protocol,
            const QString &addr,
            unsigned int port,
            unsigned int packets_count,
            const QString& measure_type,
            unsigned int delay,

            const QString &data_filename,
            unsigned int data_size,
            bool zero_copy);

    virtual std::optional<QMap<int, int>> getCPUDistribution()=0;

    virtual void setRecvSockOpt(Socket &sock)=0;
    virtual void processRecvTimestamp(msghdr &msg,
            InSystemTimeInfo &res,
            timespec &after_recv_time,
            unsigned int packets_count,
            const QString &protocol)=0;
    virtual void setSendSockOpt(Socket &sock, const QString &measure_type)=0;
    virtual bool
    processSendTimestamp(Socket &sock, InSystemTimeInfo &res, SocketOpTimestamps &timestamps, unsigned int packets_count,
                         const QString &protocol, timespec &prev) =0;
};


#endif //LFNP_BASEDATASOURCE_H
