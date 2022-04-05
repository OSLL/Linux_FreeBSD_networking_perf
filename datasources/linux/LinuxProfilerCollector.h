//
// Created by shenk on 02.04.2022.
//

#ifndef LFNP_LINUXPROFILERCOLLECTOR_H
#define LFNP_LINUXPROFILERCOLLECTOR_H

#include "../BaseProfilerCollector.h"

class LinuxProfilerCollector: public BaseProfilerCollector {

    QMap<int, QVector<FuncProfilerTreeNode*>> res;
    quint64 time;

    void onStart() override {
        time = 0;
    }

    void onTimer() override {
        ProfilerParser parser("/dev/netprofiler", time);
        time = parser.getLastTokenTimestamp();
        for (const auto cpu: parser.getAvailableCPUs()) {
            res[cpu] << parser.getProfilerTrees(cpu);
        }
    }

    QMap<int, QVector<FuncProfilerTreeNode*>> onEnd() override {
        return res;
    }

};


#endif //LFNP_LINUXPROFILERCOLLECTOR_H
