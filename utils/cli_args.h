//
// Created by shenk on 25.12.2021.
//

#ifndef LFNP_CLI_ARGS_H
#define LFNP_CLI_ARGS_H

#include <algorithm>
#include <string>

char* getCliArgument(char ** begin, char ** end, const std::string & option);

bool cliArgumentExists(char** begin, char** end, const std::string& option);

#endif //LFNP_CLI_ARGS_H
