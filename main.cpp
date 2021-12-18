#include <iostream>

#include "datasources/BaseDataSource.h"

#ifdef __linux__
#include "datasources/linux/LinuxDataSource.h"
#else
#include "datasources/freebsd/FreeBSDDataSource.h"
#endif

int main() {

#ifdef __linux__
    BaseDataSource *ds = new LinuxDataSource();
#else
    BaseDataSource *ds = new FreeBSDDataSource();
#endif

//    std::cout << ds->getTcpTotalRecv() << std::endl;
    std::cout << ds->getTcpConnList() << std::endl;

    return 0;
}
