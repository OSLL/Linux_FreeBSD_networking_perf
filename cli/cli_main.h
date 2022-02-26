//
// Created by shenk on 20.01.2022.
//

#ifndef LFNP_CLI_MAIN_H
#define LFNP_CLI_MAIN_H

#include <QCommandLineParser>
#include <QString>

#include <iostream>

#include "../datasources/BaseDataSource.h"

#ifdef __linux__
#include "../datasources/linux/LinuxDataSource.h"
#else
#include "../datasources/freebsd/FreeBSDDataSource.h"
#endif

#include "printers/printers.h"
#include "../datasources/iperf3/iperf3.h"

int cli_main(int argc, char *argv[]);

#endif //LFNP_CLI_MAIN_H
