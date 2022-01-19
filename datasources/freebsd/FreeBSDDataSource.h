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

#include "net/netisr.h"

#include "../../utils/sockets.h"
#include "../../utils/utils.h"

#include <sys/sysctl.h>

#include <iostream>
#include <map>
#include <QThread>

#include "../BaseDataSource.h"

class FreeBSDDataSource: public BaseDataSource {

private:

    static std::map<std::string, std::string> protocol_sockets_sysctl_names;
    static QMap<QString, std::tuple<QString, size_t>> protocol_stats_sysctl_names;
    static QMap<QString, QVector<QString>> protocols_stats_descriptions;

public:

    std::optional<QMap<QString, int>> getProtocolStats(const QString &protocol) override;

    std::vector<SocketInfo> getSockets(std::string protocol) override;

    std::optional<QMap<int, int>> getCPUDistribution() override;

    void setRecvSockOpt(Socket &sock) override;
    void processRecvTimestamp(msghdr &msg,
                                      InSystemTimeInfo &res,
                                      timespec &after_recv_time,
                                      unsigned int packets_count,
                                      const QString &protocol) override;
    void setSendSockOpt(Socket &sock, const QString &measure_type) override;
    bool
    processSendTimestamp(Socket &sock, InSystemTimeInfo &res, SocketOpTimestamps &timestamps, unsigned int packets_count,
                         const QString &protocol) override;
};


#endif //LFNP_FREEBSDDATASOURCE_H
