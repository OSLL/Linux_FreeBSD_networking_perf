//
// Created by shenk on 18.12.2021.
//

#include "utils.h"

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty()) {
            elems.push_back(item);
        }
    }
    return elems;
}

timespec timespecsub(timespec &tsp, timespec &usp) {
    timespec vsp {
            .tv_sec = tsp.tv_sec - usp.tv_sec,
            .tv_nsec = tsp.tv_nsec - usp.tv_nsec
    };

    if (vsp.tv_nsec < 0) {
        vsp.tv_sec--;
        vsp.tv_nsec += 1000000000L;
    }

    return vsp;
}
