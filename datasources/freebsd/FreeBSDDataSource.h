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
#include "netinet/tcp_var.h"
#include "../BaseDataSource.h"
#include <sys/sysctl.h>

class FreeBSDDataSource: public BaseDataSource {

private:

    tcpstat ts;

public:

    FreeBSDDataSource() {
        size_t size;
        sysctlbyname("net.inet.tcp.stats", &this->ts, &size, nullptr, 0);
    }

    int getTcpTotalRecv();

};


#endif //LFNP_FREEBSDDATASOURCE_H
