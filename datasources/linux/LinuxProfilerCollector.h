//
// Created by shenk on 02.04.2022.
//

#ifndef LFNP_LINUXPROFILERCOLLECTOR_H
#define LFNP_LINUXPROFILERCOLLECTOR_H

#include "../BaseProfilerCollector.h"

class LinuxProfilerCollector: public BaseProfilerCollector {

    ProfilerData res;
    quint64 time;

    void onStart() override {
        time = 0;
    }

    void onTimer() override {
        ProfilerParser parser("/dev/netprofiler", time);
        time = parser.getLastTokenTimestamp();
        for (const auto cpu: parser.getAvailableCPUs()) {
            for (const auto pid: parser.getAvailablePids(cpu)) {
                auto trees = parser.getProfilerTrees(cpu, pid);
                if (!trees.empty()) {
                    res[cpu][pid] << parser.getProfilerTrees(cpu, pid);
                }
            }
        }
    }

    ProfilerData onEnd() override {
        return res;
    }

};


#endif //LFNP_LINUXPROFILERCOLLECTOR_H
