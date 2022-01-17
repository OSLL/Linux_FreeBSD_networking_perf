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
    //TODO: Добавить время при возвращении из recv/send
    //TODO: В FreeBSD значение Total огромное. Почему?
    //TODO: Пропускать первый пакет. В протоколах без состояния происходит ожидание в recv, пока пользователь не
    // запустит measure tx-timings
    virtual std::optional<InSystemTimeInfo> recvTimestamp(
            const QString &protocol, unsigned int port, unsigned int packets_count);

    virtual std::optional<InSystemTimeInfo> sendTimestamp(
            const QString &protocol,
            const QString &addr,
            unsigned int port,
            unsigned int packets_count,
            const QString& measure_type,
            unsigned int delay);

    //TODO: рассмотреть вариант возвращаемого значения QMap. Возможно, существуют вероятности, что процессоры идут
    // не подряд
    virtual std::optional<QVector<int>> getCPUDistribution()=0;

    virtual void setRecvSockOpt(Socket &sock)=0;
    virtual void processRecvTimestamp(msghdr &msg,
            InSystemTimeInfo &res,
            timespec &after_recv_time,
            unsigned int packets_count,
            const QString &protocol)=0;
    virtual void setSendSockOpt(Socket &sock, const QString &measure_type)=0;
    virtual void processSendTimestamp(Socket &sock,
            msghdr &msg,
            InSystemTimeInfo &res,
            timespec &before_send_time,
            unsigned int packets_count,
            const QString &protocol,
            timespec &prev)=0;
};


#endif //LFNP_BASEDATASOURCE_H
