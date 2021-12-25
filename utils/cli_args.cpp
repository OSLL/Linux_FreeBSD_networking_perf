//
// Created by shenk on 25.12.2021.
//

#include "cli_args.h"

char *getCliArgument(char **begin, char **end, const std::string &option) {
    char ** itr = std::find(begin, end, option);

    if (itr != end && ++itr != end) {
        return *itr;
    }

    return NULL;
}

bool cliArgumentExists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}
