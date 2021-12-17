//
// Created by shenk on 17.12.2021.
//

#include "FreeBSDDataSource.h"

int FreeBSDDataSource::getTcpTotalRecv() {
    return this->ts.tcps_rcvtotal;
}
