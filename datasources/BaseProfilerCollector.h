//
// Created by shenk on 02.04.2022.
//

#ifndef LFNP_BASEPROFILERCOLLECTOR_H
#define LFNP_BASEPROFILERCOLLECTOR_H

#include "parsers/parsers.h"

class BaseProfilerCollector {

public:

    virtual void onStart(QVector<QString> protocols)=0;
    virtual void onTimer()=0;
    virtual ProfilerData onEnd()=0;
    virtual QStringList getSupportedProtocols()=0;

};

#endif //LFNP_BASEPROFILERCOLLECTOR_H
