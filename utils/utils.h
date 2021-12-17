//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_UTILS_H
#define LFNP_UTILS_H

#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

#endif //LFNP_UTILS_H
