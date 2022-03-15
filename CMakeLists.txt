cmake_minimum_required(VERSION 3.15)
project(lfnp)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Charts)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(lfnp main.cpp datasources/BaseDataSource.h utils/utils.h utils/utils.cpp types/SocketInfo.h
        datasources/linux/parsers/parsers.h datasources/linux/parsers/parsers.cpp cli/printers/printers.cpp
        cli/printers/printers.h types/InSystemTimeInfo.h utils/sockets.cpp utils/sockets.h
        datasources/iperf3/iperf3.cpp datasources/iperf3/iperf3.h datasources/BaseDataSource.cpp
        cli/cli_main.cpp cli/cli_main.h gui/gui_main.cpp gui/gui_main.h gui/forms/mainwindow.h
        gui/forms/mainwindow.cpp gui/forms/mainwindow.ui gui/forms/cpudistributionwidget.h
        gui/forms/cpudistributionwidget.cpp gui/forms/cpudistributionwidget.ui types/series/TimeSeries.h
        types/DropsInfo.h types/enums/CPUDistributionSource.h
        gui/forms/recvtimestampwidget.cpp gui/forms/recvtimestampwidget.h gui/forms/recvtimestampwidget.ui
        utils/default_args.h types/enums/MeasureType.h types/enums/BaseEnum.h
        datasources/timestamping/TimestampsSender.cpp datasources/timestamping/TimestampsSender.h types/TimeRange.h
        datasources/timestamping/TimestampsReceiver.cpp datasources/timestamping/TimestampsReceiver.h
        gui/threads/TimestampsReceiverThread.h types/series/FuncSeries.h types/series/BaseSeries.h
        gui/forms/sendtimestampwidget.h gui/forms/sendtimestampwidget.cpp gui/forms/sendtimestampwidget.ui
        gui/threads/TimestampsSenderThread.h types/TimestampsChart.h datasources/bandwidth/BandwidthSender.h
        datasources/bandwidth/BandwidthReceiver.h types/BandwidthResult.h gui/forms/socketslistwidget.h
        gui/forms/socketslistwidget.cpp gui/forms/socketslistwidget.ui gui/tables/SocketsListModel.h
        datasources/bandwidth/LinuxPktgen.h gui/forms/protocolsstatswidget.h gui/forms/protocolsstatswidget.cpp
        gui/forms/protocolsstatswidget.ui gui/tables/ProtocolStatsModel.h gui/tables/TableContextMenu.h
        types/MainTabWidget.h gui/forms/trackvaluewidget.h gui/forms/trackvaluewidget.cpp gui/forms/trackvaluewidget.ui
        types/AdvancedChart.h gui/forms/recvbandwidthwidget.h gui/forms/recvbandwidthwidget.cpp
        gui/forms/recvbandwidthwidget.ui gui/forms/startstopwidget.h gui/forms/startstopwidget.cpp
        gui/forms/startstopwidget.ui gui/threads/BandwidthReceiverThread.h types/enums/BandwidthUnits.h
        types/enums/UnitsPrefixes.h types/BandwidthChart.h gui/forms/sendbandwidthwidget.h
        gui/forms/sendbandwidthwidget.cpp gui/forms/sendbandwidthwidget.ui gui/threads/BandwidthSenderThread.h
        gui/forms/bandwidthwidget.cpp gui/forms/bandwidthwidget.h gui/forms/bandwidthwidget.h
        gui/forms/bandwidthwidget.cpp gui/forms/bandwidthwidget.ui datasources/netmap/netmap.h
        datasources/netmap/netmap.cpp types/FuncProfiler.h)

target_link_libraries(lfnp
        Qt5::Core
        Qt5::Widgets
        Qt5::Charts)
target_link_libraries(lfnp Threads::Threads)

find_library(DTRACE_LIBRARY NAMES dtrace)
if (DTRACE_LIBRARY)
    target_link_libraries(lfnp dtrace)
    target_sources(lfnp PUBLIC datasources/dtrace/DTrace.h datasources/dtrace/DTrace.cpp)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
    target_sources(lfnp PUBLIC datasources/linux/LinuxDataSource.h datasources/linux/LinuxDataSource.cpp)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    target_sources(lfnp PUBLIC datasources/freebsd/FreeBSDDataSource.h datasources/freebsd/FreeBSDDataSource.cpp
            datasources/freebsd/protocols_stats_funcs.h datasources/freebsd/protocols_stats_funcs.cpp)
endif()
