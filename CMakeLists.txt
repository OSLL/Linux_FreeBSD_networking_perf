cmake_minimum_required(VERSION 3.15)
project(lfnp)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Charts)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(lfnp main.cpp datasources/BaseDataSource.h utils/utils.h utils/utils.cpp types/SocketInfo.h
        datasources/linux/parsers/parsers.h datasources/linux/parsers/parsers.cpp cli/printers/printers.cpp
        cli/printers/printers.h types/InSystemTimeInfo.h utils/sockets.cpp utils/sockets.h
        datasources/iperf3/iperf3_parser.cpp datasources/iperf3/iperf3_parser.h datasources/BaseDataSource.cpp
        cli/cli_main.cpp cli/cli_main.h gui/gui_main.cpp gui/gui_main.h gui/forms/mainwindow.h
        gui/forms/mainwindow.cpp gui/forms/mainwindow.ui gui/forms/cpudistributionwidget.h
        gui/forms/cpudistributionwidget.cpp gui/forms/cpudistributionwidget.ui types/series/TimeSeries.h
        types/DynamicAxisChart.h types/DropsInfo.h types/enums/CPUDistributionSource.h
        gui/forms/recvtimestampwidget.cpp gui/forms/recvtimestampwidget.h gui/forms/recvtimestampwidget.ui
        utils/default_args.h types/enums/MeasureType.h types/enums/BaseEnum.h
        datasources/timestamping/TimestampsSender.cpp datasources/timestamping/TimestampsSender.h types/TimeRange.h
        datasources/timestamping/TimestampsReceiver.cpp datasources/timestamping/TimestampsReceiver.h
        gui/threads/TimestampsReceiverThread.h types/series/FuncSeries.h types/series/BaseSeries.h
        gui/forms/sendtimestampwidget.h gui/forms/sendtimestampwidget.cpp gui/forms/sendtimestampwidget.ui gui/threads/TimestampsSenderThread.h)

target_link_libraries(lfnp
        Qt5::Core
        Qt5::Widgets
        Qt5::Charts)

if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
    target_sources(lfnp PUBLIC datasources/linux/LinuxDataSource.h datasources/linux/LinuxDataSource.cpp)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    target_sources(lfnp PUBLIC datasources/freebsd/FreeBSDDataSource.h datasources/freebsd/FreeBSDDataSource.cpp)
endif()
