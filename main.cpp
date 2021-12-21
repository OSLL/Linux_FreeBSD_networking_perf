#include <iostream>

#include "datasources/BaseDataSource.h"

#ifdef __linux__
#include "datasources/linux/LinuxDataSource.h"
#else
#include "datasources/freebsd/FreeBSDDataSource.h"
#endif

int main(int argc, char *argv[]) {

#ifdef __linux__
    BaseDataSource *ds = new LinuxDataSource();
#else
    BaseDataSource *ds = new FreeBSDDataSource();
#endif

    if (argc > 1 && std::string(argv[1]) == "list") {
        if (argc > 2 && std::string(argv[2]) == "sockets") {
            if (argc == 4) {
                ds->getSockets(argv[3]);
            } else {
                std::cout << "Please, specify protocol" << std::endl;
            }
        }
    }

//    std::cout << ds->getTcpTotalRecv() << std::endl;
//    std::cout << ds->getTcpConnList() << std::endl;

    return 0;
}
