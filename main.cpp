#include <iostream>

#include "datasources/BaseDataSource.h"

#ifdef __linux__
#include "datasources/linux/LinuxDataSource.h"
#else
#include "datasources/freebsd/FreeBSDDataSource.h"
#endif

#include "printers/printers.h"

#include <QCommandLineParser>

int main(int argc, char *argv[]) {

    QCommandLineParser parse;

#ifdef __linux__
    BaseDataSource *ds = new LinuxDataSource();
#else
    BaseDataSource *ds = new FreeBSDDataSource();
#endif

    if (argc > 1 && std::string(argv[1]) == "list") {
        if (argc > 2 && std::string(argv[2]) == "sockets") {
            if (argc == 4) {
                printSocketsInfoList(ds->getSockets(argv[3]));
            } else {
                std::cout << "Please, specify protocol" << std::endl;
            }
        }
    } else if (argc > 1 && std::string(argv[1]) == "measure") {
        if (argc > 2 && std::string(argv[2]) == "rx-timings") {
            auto rx_time = ds->getInSystemTimeRX("udp", 10000).value();

            std::cout << "Hardware: "
                      << rx_time.rx_hardware_kernel_time.tv_sec << " sec "
                      << rx_time.rx_hardware_kernel_time.tv_nsec << " ns" << std::endl;

            std::cout << "Sofrware: "
                      << rx_time.rx_kernel_user_time.tv_sec << " sec "
                      << rx_time.rx_kernel_user_time.tv_nsec << " ns" << std::endl;

        } else if (argc > 2 && std::string(argv[2]) == "tx-timings") {
            ds->sendTimestamp("raw", 10000);
        }
    }

//    std::cout << ds->getTcpTotalRecv() << std::endl;
//    std::cout << ds->getTcpConnList() << std::endl;

    return 0;
}
