//
// Created by shenk on 15.03.2022.
//

#include "DTrace.h"
#include <QDebug>
#include <QThread>
#include <QTemporaryFile>

DTrace::DTrace(): program_text("") {

    int error = 0;
    dtrace = dtrace_open(DTRACE_VERSION, 0, &error);

    dtrace_probe_iter(dtrace, NULL, dtrace_probe_func, this);

    error = dtrace_setopt(dtrace, "bufsize", "512k");
    if (error == -1) {
        int dt_error = dtrace_errno(dtrace);
        qDebug() << "Error: dtrace_setopt " << dtrace_errmsg(dtrace, dt_error);
    }

    error = dtrace_setopt(dtrace, "aggsize", "512k");
    if (error == -1) {
        int dt_error = dtrace_errno(dtrace);
        qDebug() << "Error: dtrace_setopt " << dtrace_errmsg(dtrace, dt_error);
    }

}

DTrace::~DTrace() {

    dtrace_close(dtrace);

}

void DTrace::start() {
    int error = 0;

    dtrace_prog_t *prog = dtrace_program_strcompile(dtrace, program_text.toLocal8Bit().data(),
                                                    DTRACE_PROBESPEC_NAME, 0, 0, NULL);

    dtrace_proginfo_t info;
    error = dtrace_program_exec(dtrace, prog, &info);
    if (error == -1) {
        int dt_error = dtrace_errno(dtrace);
        qDebug() << "Error: dtrace_program_exec " << dtrace_errmsg(dtrace, dt_error);
    }

    error = dtrace_go(dtrace);
    if (error == -1) {
        int dt_error = dtrace_errno(dtrace);
        qDebug() << "Error: dtrace_go " << dtrace_errmsg(dtrace, dt_error);
    }
}

void DTrace::work(FILE *file) {
    dtrace_work(dtrace, file, dtrace_process, dtrace_process_rec, this);
}

void DTrace::stop() {
    int error = dtrace_stop(dtrace);
    if (error == -1) {
        int dt_error = dtrace_errno(dtrace);
        qDebug() << "Error: dtrace_stop " << dtrace_errmsg(dtrace, dt_error);
    }
}

#define PROFILER(func_name) "fbt:kernel:" + func_name + ":entry {printf(\"enter " + func_name + " %d %d\\n\", timestamp, cpu);} " \
"fbt:kernel:" + func_name + ":return {printf(\"return " + func_name + " %d %d\\n\", timestamp, cpu);}"
void DTrace::addProbe(const QString &func_name) {

    if (probes.contains(func_name)) {
        program_text += PROFILER(func_name);
    }

}
#undef PROFILER

