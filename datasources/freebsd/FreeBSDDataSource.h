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

#include "netinet/ip.h"
#include "netinet/udp.h"
#include "netinet/ip_icmp.h"

#include "netinet/tcp_var.h"
#include "netinet/udp_var.h"
#include "netinet/igmp_var.h"
#include "netinet/icmp_var.h"
#include "netinet/ip_var.h"

#include "netinet6/ip6_var.h"
#include "netinet6/raw_ip6.h"
#include "netinet/icmp6.h"

#include "net/netisr.h"

#include "../../utils/sockets.h"
#include "../../utils/utils.h"
#include "protocols_stats_funcs.h"

#include <sys/sysctl.h>
#include <net/if.h>
#include <ifaddrs.h>

#include <iostream>
#include <map>

#include <QThread>
#include <QRegularExpression>

#include "../BaseDataSource.h"
#include "../dtrace/DTrace.h"
#include "../parsers/parsers.h"

class FreeBSDDataSource: public BaseDataSource {

private:

    static QMap<QString, QString> protocol_sockets_sysctl_names;
    static QMap<QString, std::tuple<QString, size_t, std::function<ProtocolStats(char*)>>> protocol_stats_sysctl_names;

public:

    std::optional<ProtocolStats> getProtocolStats(const QString &protocol) override;

    QVector<SocketInfo> getSockets(QString protocol) override;

    std::optional<QMap<int, int>> getCPUDistribution(CPUDistributionSource source) override;

    std::optional<QMap<QString, DropsInfo>> getDevsDropsInfo() override;

    QVector<QPair<QString, DropsInfo>> getDropsInfo() override;

    void setRecvSockOpt(Socket &sock) override;
    void processRecvTimestamp(msghdr &msg, ReceiveTimestamp &res, timespec &after_recv_time,
                              const Socket &sock) override;

    void setSendSockOpt(Socket &sock, const MeasureType measure_type) override;
    void
    processSendTimestamp(Socket &sock, SendTimestamp &res, TimeRange &timestamps) override;

    QStringList getSupportedSocketsListProtocols() override;
    QStringList getSupportedStatsProtocols() override;

    std::optional<ProfilerParser> getProfilerData() override;
};


#endif //LFNP_FREEBSDDATASOURCE_H
