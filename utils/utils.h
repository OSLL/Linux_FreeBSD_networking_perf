//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_UTILS_H
#define LFNP_UTILS_H

#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> split(const std::string &s, char delim);

timespec timespecsub(timespec &tsp, timespec &usp);

#endif //LFNP_UTILS_H
