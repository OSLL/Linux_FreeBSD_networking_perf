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
    QString program_text;
    QSet<QString> probes;

    static int dtrace_probe_func(dtrace_hdl_t * dtrace, const dtrace_probedesc_t *desc, void *args) {
        auto dTrace = (DTrace *)args;
        dTrace->probes.insert(desc->dtpd_func);
        return 0;
    }

    static int dtrace_process_rec(const dtrace_probedata_t *data, const dtrace_recdesc_t *rec, void *arg){
        return DTRACE_CONSUME_THIS;
    }

    static int dtrace_process(const dtrace_probedata_t *data, void *arg){
        return DTRACE_CONSUME_THIS;
    }

public:

    DTrace();

    void start();
    void work(FILE *file);
    void stop();

    void addProbe(const QString &func_name);
    ~DTrace();

};


#endif //LFNP_DTRACE_H
