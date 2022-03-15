//
// Created by shenk on 15.03.2022.
//

#include "DTrace.h"
#include <QDebug>
#include <QThread>
#include <QTemporaryFile>

DTrace::DTrace() {

    int error = 0;
    dtrace = dtrace_open(DTRACE_VERSION, 0, &error);

    dtrace_prog_t *prog = dtrace_program_strcompile(dtrace,
                                                   "fbt:kernel:tcp_input:entry {printf(\"enter tcp_input %d %d\\n\", timestamp, cpu);} "
                                                   "fbt:kernel:tcp_input:return {printf(\"return tcp_input %d %d\\n\", timestamp, cpu);}",
                                                    DTRACE_PROBESPEC_NAME, 0, 0, NULL);

    dtrace_proginfo_t info;
    error = dtrace_program_exec(dtrace, prog, &info);
    if (error == -1) {
        int dt_error = dtrace_errno(dtrace);
        qDebug() << "Error: dtrace_program_exec " << dtrace_errmsg(dtrace, dt_error);
    }

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

std::unique_ptr<QFile> DTrace::start() {
    int error = 0;
    FILE* tmp_file = tmpfile();

    error = dtrace_go(dtrace);
    if (error == -1) {
        int dt_error = dtrace_errno(dtrace);
        qDebug() << "Error: dtrace_go " << dtrace_errmsg(dtrace, dt_error);
    }

    for (int i=0; i<3; i++) {
        dtrace_sleep(dtrace);
        dtrace_work(dtrace, tmp_file, dtrace_process, dtrace_process_rec, this);
    }

    error = dtrace_stop(dtrace);
    if (error == -1) {
        int dt_error = dtrace_errno(dtrace);
        qDebug() << "Error: dtrace_stop " << dtrace_errmsg(dtrace, dt_error);
    }

    auto file = std::make_unique<QFile>();
    file->open(tmp_file, QIODevice::ReadOnly);
    file->reset();

    return file;
}

