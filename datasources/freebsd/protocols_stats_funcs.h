//
// Created by shenk on 01.02.2022.
//

#ifndef LFNP_PROTOCOLS_STATS_FUNCS_H
#define LFNP_PROTOCOLS_STATS_FUNCS_H

#include "../BaseDataSource.h"

#include "netinet/ip.h"
#include "netinet/udp.h"
#include "netinet/ip_icmp.h"

#include "netinet/ip_var.h"
#include "netinet/tcp_var.h"
#include "netinet/udp_var.h"
#include "netinet/igmp_var.h"
#include "netinet/icmp_var.h"

#define ADD_STAT(title, name) stats[#title] = data->name

ProtocolStats ip_stats(char *_data);
ProtocolStats tcp_stats(char *_data);
ProtocolStats udp_stats(char *_data);
ProtocolStats icmp_stats(char *_data);
ProtocolStats igmp_stats(char *_data);

#endif //LFNP_PROTOCOLS_STATS_FUNCS_H
