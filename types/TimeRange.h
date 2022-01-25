//
// Created by shenk on 25.01.2022.
//

#ifndef LFNP_TIMERANGE_H
#define LFNP_TIMERANGE_H

#include <ctime>
#include <QtGlobal>

class TimeRange {

public:

    TimeRange(): from({0, 0}), to({0, 0}) {}
    TimeRange(timespec _from, timespec _to): from(_from), to(_to) {}
    quint64 getRangeNS() {

        quint64 range = 0;
        range += to.tv_nsec - from.tv_nsec;
        range += (to.tv_sec - from.tv_sec) * 1000000000L;

        return range;
    }

    timespec from;
    timespec to;
};


#endif //LFNP_TIMERANGE_H
