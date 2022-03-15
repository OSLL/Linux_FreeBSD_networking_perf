//
// Created by shenk on 15.03.2022.
//

#ifndef LFNP_DTRACE_H
#define LFNP_DTRACE_H

#include "dtrace.h"

class DTrace {

private:

    dtrace_hdl_t *dtrace;

public:

    DTrace();
    ~DTrace();

};


#endif //LFNP_DTRACE_H
