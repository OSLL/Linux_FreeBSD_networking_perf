//
// Created by shenk on 20.01.2022.
//

#include "cli_main.h"

int cli_main(int argc, char *argv[]) {
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
                        {"measure-type", R"(Type of measure: "software" or "scheduler")","measure-type", "software"},
                        {"delay", "Delay between sending timestamps, ms","delay", "0"},
                        {"ns", "Output in nanoseconds"},
                        {"data", "File, from which taken data to send", "data", "/dev/urandom"},
                        {"data-size", "Count of bytes to send. If no specified, send all file", "data-size", "0"},
                        {"zero-copy", "If specified, use sendfile call"}
                });

        parser.process(args);

        auto protocol = parser.value("protocol");
        auto port = parser.value("port").toUInt();
        auto packets_count = parser.value("packets-count").toUInt();
        auto addr = parser.value("address");
        auto measure_type = parser.value("measure-type");
        auto in_ms = !parser.isSet("ns");
        auto delay = parser.value("delay").toUInt();
        auto data_filename = parser.value("data");
        auto data_size = parser.value("data-size").toUInt();
        auto zero_copy = parser.isSet("zero-copy");

        if (data_filename == "/dev/urandom" && !data_size) {
            data_size = 1000;
        }

        if (argc > 2 && args[2] == "rx-timings") {

            auto o_rx_time = ds->recvTimestamp(protocol, port, packets_count);
            printInSystemTimeInfo(o_rx_time, in_ms);

        } else if (argc > 2 && std::string(argv[2]) == "tx-timings") {

            auto o_tx_time = ds->sendTimestamp(protocol, addr, port, packets_count, measure_type, delay,
                                               data_filename, data_size, zero_copy);
            printInSystemTimeInfo(o_tx_time, in_ms);

        }
    } else if (argc > 2 && args[1] == "iperf3") {
        printIperf3Data(parseIperf3Output(args[2]));
    } else if (argc > 2 && args[1] == "stats") {
        printProtocolStats(ds->getProtocolStats(args[2]));
    } else if (argc > 1 && args[1] == "get-cpu-distribution") {
        printCPUDistribution(ds->getCPUDistribution());
    } else if (argc > 1 && args[1] == "get-drops-info") {
        printDropsInfo(ds->getDropsInfo());
    }

    return 0;
}
