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

    QStringList args(argv, argv+argc);

    QCommandLineParser parser;

#ifdef __linux__
    BaseDataSource *ds = new LinuxDataSource();
#else
    BaseDataSource *ds = new FreeBSDDataSource();
#endif

    if (argc > 1 && args[1] == "list") {
        if (argc > 2 && args[2] == "sockets") {
            if (argc == 4) {
                printSocketsInfoList(ds->getSockets(argv[3]));
            } else {
                std::cout << "Please, specify protocol" << std::endl;
            }
        }
    } else if (argc > 1 && args[1] == "measure") {

        parser.addOptions(
                {
                        {"protocol", "Use specified protocol.", "protocol", "tcp"},
                        {{"p", "port"}, "Use specified port.", "port", "7435"},
                        {"packets-count", "Receive specified count of packets.", "packets-count", "100"},
                        {{"a", "address"}, "Send packets to specified address.","address", "127.0.0.1"}
                });

        parser.process(args);

        auto protocol = parser.value("protocol");
        auto port = parser.value("port").toUInt();
        auto packets_count = parser.value("packets-count").toUInt();
        auto addr = parser.value("address");

        if (argc > 2 && args[2] == "rx-timings") {

            auto rx_time = ds->getInSystemTimeRX(protocol, port, packets_count).value();

            std::cout << "Hardware: "
                      << rx_time.rx_hardware_kernel_time.tv_sec << " sec "
                      << rx_time.rx_hardware_kernel_time.tv_nsec << " ns" << std::endl;

            std::cout << "Sofrware: "
                      << rx_time.rx_kernel_user_time.tv_sec << " sec "
                      << rx_time.rx_kernel_user_time.tv_nsec << " ns" << std::endl;

        } else if (argc > 2 && std::string(argv[2]) == "tx-timings") {
            ds->sendTimestamp(protocol, addr, port, packets_count);
        }
    }

    return 0;
}
