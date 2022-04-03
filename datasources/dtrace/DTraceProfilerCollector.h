//
// Created by shenk on 02.04.2022.
//

#ifndef LFNP_DTRACEPROFILERCOLLECTOR_H
#define LFNP_DTRACEPROFILERCOLLECTOR_H

#include "../BaseProfilerCollector.h"
#include "DTrace.h"

#define PROFILER(func_name) dTrace.addProbe(func_name);

class DTraceProfilerCollector: public BaseProfilerCollector {

    DTrace dTrace;
    FILE* tmp_file;

    void onStart() override {

        tmp_file = tmpfile();

        PROFILER("ip_initid")
        PROFILER("ip_findroute")
        PROFILER("ip_output_send")
        PROFILER("ip_ecn_egress")
        PROFILER("ip_ecn_ingress")
        PROFILER("ip_checkrouteralert")
        PROFILER("ip_msource_tree_RB_MINMAX")
        PROFILER("ip_msource_tree_RB_NEXT")
        PROFILER("ip_output")
        PROFILER("ip_fillid")
        PROFILER("ip_mfilter_alloc")
        PROFILER("ip_msource_tree_RB_FIND")
        PROFILER("ip_msource_tree_RB_INSERT")
        PROFILER("ip_msource_tree_RB_INSERT_COLOR")
        PROFILER("ip_msource_tree_RB_NFIND")
        PROFILER("ip_msource_tree_RB_PREV")
        PROFILER("ip_msource_tree_RB_REINSERT")
        PROFILER("ip_msource_tree_RB_REMOVE")
        PROFILER("ip_msource_tree_RB_REMOVE_COLOR")
        PROFILER("ip_init")
        PROFILER("ip_fragment")
        PROFILER("ip_tryforward")
        PROFILER("ip_next_mtu")
        PROFILER("ip_srcroute")
        PROFILER("ip_dooptions")
        PROFILER("ip_forward")
        PROFILER("ip_input")
        PROFILER("ip_reass")
        PROFILER("ip_rsvp_done")
        PROFILER("ip_rsvp_init")
        PROFILER("ip_savecontrol")
        PROFILER("ip_insertoptions")
        PROFILER("ip_optcopy")
        PROFILER("ip_pcbopts")
        PROFILER("ip_ctloutput")

        dTrace.start();
    }

    void onTimer() override {
        dTrace.work(tmp_file);
    }

    ProfilerParser onEnd() override {
        dTrace.stop();

        auto file = std::make_unique<QFile>();
        file->open(tmp_file, QIODevice::ReadOnly);
        file->reset();
        QTextStream stream(file.get());
        return ProfilerParser(stream);
    }

};

#undef PROFILER
#endif //LFNP_DTRACEPROFILERCOLLECTOR_H
