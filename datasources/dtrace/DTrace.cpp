//
// Created by shenk on 15.03.2022.
//

#include "DTrace.h"

DTrace::DTrace() {

    int error = 0;
    dtrace = dtrace_open(DTRACE_VERSION, 0, &error);

}

DTrace::~DTrace() {

    dtrace_close(dtrace);

}
