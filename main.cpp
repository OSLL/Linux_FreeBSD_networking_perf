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
                        {{"a", "address"}, "Send packets to specified address.","address", "127.0.0.1"},
                        {"measure-type", R"(Type of measure: "software" or "scheduler")","measure-type", "software"}
                });

        parser.process(args);

        auto protocol = parser.value("protocol");
        auto port = parser.value("port").toUInt();
        auto packets_count = parser.value("packets-count").toUInt();
        auto addr = parser.value("address");
        auto measure_type = parser.value("measure-type");

        if (argc > 2 && args[2] == "rx-timings") {

            auto o_rx_time = ds->getInSystemTimeRX(protocol, port, packets_count);
            if (o_rx_time) {
                std::cout << "Hardware: "
                          << o_rx_time->rx_hardware_time.tv_sec << " sec "
                          << o_rx_time->rx_hardware_time.tv_nsec << " ns" << std::endl;

                std::cout << "Software: "
                          << o_rx_time->rx_software_time.tv_sec << " sec "
                          << o_rx_time->rx_software_time.tv_nsec << " ns" << std::endl;
            } else {
                std::cout << "Can't measure rx timings" << std::endl;
            }

        } else if (argc > 2 && std::string(argv[2]) == "tx-timings") {
            auto o_tx_time = ds->sendTimestamp(protocol, addr, port, packets_count, measure_type);

            if (o_tx_time) {
                std::cout << "Hardware: "
                          << o_tx_time->tx_hardware_time.tv_sec << " sec "
                          << o_tx_time->tx_hardware_time.tv_nsec << " ns" << std::endl;

                std::cout << "Software: "
                          << o_tx_time->tx_software_time.tv_sec << " sec "
                          << o_tx_time->tx_software_time.tv_nsec << " ns" << std::endl;
            } else {
                std::cout << "Can't measure tx timings" << std::endl;
            }
        }
    }

    return 0;
}
