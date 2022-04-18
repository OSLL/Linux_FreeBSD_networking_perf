//
// Created by shenk on 20.01.2022.
//

#include "cli_main.h"
#include "../utils/default_args.h"
#include "../datasources/bandwidth/LinuxPktgen.h"
#include <QThread>

int cli_main(int argc, char *argv[]) {
    QStringList args(argv, argv+argc);

    QCommandLineParser parser;
    parser.addOptions(
            {
                    {"protocol", "Use specified protocol.", "protocol", default_args["protocol"]},
                    {{"p", "port"}, "Use specified port.", "port", default_args["port"]},
                    {"packets-count", "Receive specified count of packets.", "packets-count", default_args["packets-count"]},
                    {{"a", "address"}, "Send packets to specified address.","address", default_args["address"]},
                    {"measure-type", R"(Type of measure: "software_recv" or "scheduler")","measure-type", default_args["measure-type"]},
                    {"delay", "Delay between sending timestamps, ms","delay", default_args["delay"]},
                    {"ns", "Output in nanoseconds"},
                    {"data", "File, from which taken data to send", "data", default_args["data"]},
                    {"data-size", "Count of bytes to send. If no specified, send all file", "data-size", default_args["data-size"]},
                    {"zero-copy", "If specified, use sendfile call"},
                    {{"source", "s"}, "Source for get CPU distribution",
                            "source",
#ifdef __linux__
                            "net-rx"
#else
                            "netisr"
#endif
                    },
                    {"duration", "Duration of bandwidth measure", "duration", default_args["duration"]},
                    {"threads", "Count of threads to use", "threads", default_args["threads"]},
                    {"mac", "MAC address to send data", "mac", default_args["mac"]},
                    {{"i", "interface"}, "Interface to send data", "interface", default_args["interface"]},
                    {"cpu-affinity", "If set, bind relevant threads of receiver and sender to one cpu"}
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
    auto data_size = parser.value("data-size").toULongLong();
    auto zero_copy = parser.isSet("zero-copy");

    auto s_source = parser.value("source");
    auto o_source = cpu_distribution_source_enum.fromString(s_source);

    auto duration = parser.value("duration").toULongLong();
    auto threads_count = parser.value("threads").toULongLong();

    auto mac_addr = parser.value("mac");
    auto interface = parser.value("interface");
    auto cpu_affinity = parser.isSet("cpu-affinity");

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

        if (argc > 2 && args[2] == "rx-timings") {

            auto o_rx_time = ds->recvTimestamps(protocol, port, packets_count);
            printTimestampsAverage(o_rx_time, in_ms);

        } else if (argc > 2 && args[2] == "tx-timings") {

            auto o_tx_time = ds->sendTimestamps(protocol, addr, port, packets_count, measure_type, delay,
                                                data_filename, data_size, zero_copy);
            printTimestampsAverage(o_tx_time, in_ms);

        } else if (argc > 2 && args[2] == "tx-bandwidth") {
            printBandwidthResult(ds->sendBandwidth(protocol, addr, port, duration, data_filename, data_size, zero_copy, threads_count, cpu_affinity));
        } else if (argc > 2 && args[2] == "rx-bandwidth") {
            printBandwidthResult(ds->recvBandwidth(protocol, port, threads_count, cpu_affinity));
        }
    } else if (argc > 2 && args[1] == "stats") {
        printProtocolStats(ds->getProtocolStats(args[2]));
    } else if (argc > 1 && args[1] == "get-cpu-distribution") {

        if (o_source) {
            printCPUDistribution(ds->getCPUDistribution(*o_source));
        } else {
            std::cout << "Undefined source" << std::endl;
        }
    } else if (argc > 1 && args[1] == "drops") {
        printDropsInfo(ds->getDropsInfo());
    } else if (argc > 1 && args[1] == "pktgen") {
        auto pktgen = LinuxPktgen(protocol, interface, addr, port, mac_addr, threads_count, data_size, packets_count);
        pktgen.start();
        printBandwidthResult(pktgen.getResult());
    } else if (argc > 1 && args[1] == "profiler") {
        printProfilerData(ds->getProfilerData(protocol));
    } else {
        std::cout << "Unknown command";
    }

    return 0;
}
