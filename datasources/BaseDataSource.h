//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_BASEDATASOURCE_H
#define LFNP_BASEDATASOURCE_H

#include <vector>

#include "../types/SocketInfo.h"
#include "../types/InSystemTimeInfo.h"

class BaseDataSource {

public:

    virtual int getTcpTotalRecv()=0;

    virtual std::vector<SocketInfo> getSockets(std::string protocol)=0;

    virtual InSystemTimeInfo getInSystemTime()=0;

};


#endif //LFNP_BASEDATASOURCE_H
