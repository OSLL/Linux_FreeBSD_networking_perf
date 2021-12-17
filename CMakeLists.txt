cmake_minimum_required(VERSION 3.15)
project(lfnp)

set(CMAKE_CXX_STANDARD 17)

add_executable(lfnp main.cpp datasources/BaseDataSource.h utils/utils.h )

if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
    target_sources(lfnp PUBLIC datasources/linux/LinuxDataSource.h datasources/linux/LinuxDataSource.cpp)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    target_sources(lfnp PUBLIC datasources/freebsd/FreeBSDDataSource.h datasources/freebsd/FreeBSDDataSource.cpp)
endif()