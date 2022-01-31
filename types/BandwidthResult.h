//
// Created by shenk on 27.01.2022.
//

#ifndef LFNP_BANDWIDTHRESULT_H
#define LFNP_BANDWIDTHRESULT_H

#include <QtGlobal>

struct BandwidthResult {

    quint64 packets_count;
    quint64 bytes_count;
    quint64 duration;

    BandwidthResult(): packets_count(0), bytes_count(0), duration(0) {}
    BandwidthResult(quint64 _packets_count, quint64 _bytes_count, quint64 _duration):
    packets_count(_packets_count),
    bytes_count(_bytes_count),
    duration(_duration) {}

};

#endif //LFNP_BANDWIDTHRESULT_H
