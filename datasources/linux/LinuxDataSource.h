//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_LINUXDATASOURCE_H
#define LFNP_LINUXDATASOURCE_H


#include "../BaseDataSource.h"

class LinuxDataSource: public BaseDataSource {

public:

    LinuxDataSource();

    int getTcpTotalRecv();

};


#endif //LFNP_LINUXDATASOURCE_H
