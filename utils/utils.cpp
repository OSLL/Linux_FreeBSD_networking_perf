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

bool is_timespec_empty(timespec &tsp) {
    return (tsp.tv_sec == 0 && tsp.tv_nsec == 0);
}

void timespec_avg_add(timespec &res, timespec &from, timespec &to, unsigned int total_count) {

    // Так как from и to - время. поулченное от системы и from - от сокета, то они могут быть пустыми (то есть
    // timestamp не поддерживается системой). В таком случае timespecsub не должно быть вызвано, так как получим
    // отрицательное время
    if (!is_timespec_empty(from) && !is_timespec_empty(to)) {
        auto diff = timespecsub(to, from);
        res.tv_sec += diff.tv_sec / total_count;
        res.tv_nsec += diff.tv_nsec / total_count;
    }

}
