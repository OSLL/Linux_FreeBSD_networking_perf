//
// Created by shenk on 15.03.2022.
//

#ifndef LFNP_DTRACE_H
#define LFNP_DTRACE_H

#include "dtrace.h"
#include "QDebug"
#include "QFile"

class DTrace {

private:

    dtrace_hdl_t *dtrace;

    static int dtrace_process_rec(const dtrace_probedata_t *data, const dtrace_recdesc_t *rec, void *arg){
        return DTRACE_CONSUME_THIS;
    }

    static int dtrace_process(const dtrace_probedata_t *data, void *arg){
        return DTRACE_CONSUME_THIS;
    }

public:

    DTrace();
    std::unique_ptr<QFile> start();
    ~DTrace();

};


#endif //LFNP_DTRACE_H
