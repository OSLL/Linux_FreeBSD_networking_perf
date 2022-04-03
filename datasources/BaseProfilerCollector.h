//
// Created by shenk on 02.04.2022.
//

#ifndef LFNP_BASEPROFILERCOLLECTOR_H
#define LFNP_BASEPROFILERCOLLECTOR_H

#include "parsers/parsers.h"

class BaseProfilerCollector {

public:

    virtual void onStart()=0;
    virtual void onTimer()=0;
    virtual ProfilerParser onEnd()=0;

};

#endif //LFNP_BASEPROFILERCOLLECTOR_H
