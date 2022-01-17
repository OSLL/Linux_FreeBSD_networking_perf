cmake_minimum_required(VERSION 3.15)
project(lfnp)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Core REQUIRED)
add_executable(lfnp main.cpp datasources/BaseDataSource.h utils/utils.h utils/utils.cpp types/SocketInfo.h datasources/linux/parsers/parsers.h datasources/linux/parsers/parsers.cpp printers/printers.cpp printers/printers.h types/InSystemTimeInfo.h utils/sockets.cpp utils/sockets.h datasources/iperf3/iperf3_parser.cpp datasources/iperf3/iperf3_parser.h datasources/BaseDataSource.cpp)
target_link_libraries(lfnp Qt5::Core)

if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
    target_sources(lfnp PUBLIC datasources/linux/LinuxDataSource.h datasources/linux/LinuxDataSource.cpp)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    target_sources(lfnp PUBLIC datasources/freebsd/FreeBSDDataSource.h datasources/freebsd/FreeBSDDataSource.cpp)
endif()