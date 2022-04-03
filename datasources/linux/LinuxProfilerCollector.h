//
// Created by shenk on 02.04.2022.
//

#ifndef LFNP_LINUXPROFILERCOLLECTOR_H
#define LFNP_LINUXPROFILERCOLLECTOR_H

#include "../BaseProfilerCollector.h"

class LinuxProfilerCollector: public BaseProfilerCollector {

    ProfilerParser parser;

    void onStart() override {}

    void onTimer() override {
        ProfilerParser current_parser("/dev/netprofiler");
        parser.addData(current_parser);
    }

    ProfilerParser onEnd() override {
        return parser;
    }

};


#endif //LFNP_LINUXPROFILERCOLLECTOR_H
