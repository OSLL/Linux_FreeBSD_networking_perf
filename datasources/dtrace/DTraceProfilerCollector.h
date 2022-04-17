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

    void onStart(QVector<QString> protocols) override {

        tmp_file = tmpfile();

        if (protocols.contains("ip")) {
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
        }

        if (protocols.contains("ip6")) {
            PROFILER("ip6_findroute")
            PROFILER("ip6_output_delayed_csum")
            PROFILER("ip6_output_send")
            PROFILER("ip6_setpktopt")
            PROFILER("ip6_splithdr")
            PROFILER("ip6_lasthdr")
            PROFILER("ip6_sprintf")
            PROFILER("ip6_ecn_egress")
            PROFILER("ip6_ecn_ingress")
            PROFILER("ip6_freemoptions")
            PROFILER("ip6_freepcbopts")
            PROFILER("ip6_optlen")
            PROFILER("ip6_output")
            PROFILER("ip6_copypktopts")
            PROFILER("ip6_randomflowlabel")
            PROFILER("ip6_ctloutput")
            PROFILER("ip6_savecontrol_v4")
            PROFILER("ip6_unknown_opt")
            PROFILER("ip6_deletefraghdr")
            PROFILER("ip6_get_prevhdr")
            PROFILER("ip6_savecontrol")
            PROFILER("ip6_getmoptions")
            PROFILER("ip6_mfilter_alloc")
            PROFILER("ip6_msource_tree_RB_FIND")
            PROFILER("ip6_msource_tree_RB_INSERT")
            PROFILER("ip6_msource_tree_RB_INSERT_COLOR")
            PROFILER("ip6_msource_tree_RB_MINMAX")
            PROFILER("ip6_msource_tree_RB_NEXT")
            PROFILER("ip6_msource_tree_RB_NFIND")
            PROFILER("ip6_msource_tree_RB_PREV")
            PROFILER("ip6_msource_tree_RB_REINSERT")
            PROFILER("ip6_msource_tree_RB_REMOVE")
            PROFILER("ip6_msource_tree_RB_REMOVE_COLOR")
            PROFILER("ip6_setmoptions")
            PROFILER("ip6_notify_pmtu")
            PROFILER("ip6_init")
            PROFILER("ip6_tryforward")
            PROFILER("ip6_forward")
            PROFILER("ip6_input")
            PROFILER("ip6_nexthdr")
            PROFILER("ip6_process_hopopts")
            PROFILER("ip6_clearpktopts")
            PROFILER("ip6_fragment")
            PROFILER("ip6_initpktopts")
            PROFILER("ip6_mloopback")
            PROFILER("ip6_raw_ctloutput")
            PROFILER("ip6_setpktopts")
        }

        if (protocols.contains("tcp")) {
            PROFILER("tcp_log_dev_clear_cdevpriv")
            PROFILER("tcp_log_dev_ioctl")
            PROFILER("tcp_log_dev_modevent")
            PROFILER("tcp_log_dev_open")
            PROFILER("tcp_log_dev_poll")
            PROFILER("tcp_log_dev_read")
            PROFILER("tcp_log_dev_write")
            PROFILER("tcp_fastopen_autokey_callout")
            PROFILER("tcp_fastopen_ccache_bucket_trim")
            PROFILER("tcp_fastopen_ccache_create")
            PROFILER("tcp_fastopen_ccache_lookup")
            PROFILER("tcp_hc_insert")
            PROFILER("tcp_hc_lookup")
            PROFILER("tcp_hc_purge")
            PROFILER("tcp_hc_purge_internal")
            PROFILER("tcp_vnet_init")
            PROFILER("tcp_vnet_uninit")
            PROFILER("tcp_log_copyout")
            PROFILER("tcp_log_dump_node_logbuf")
            PROFILER("tcp_log_expandlogbuf")
            PROFILER("tcp_log_expire")
            PROFILER("tcp_log_free_queue")
            PROFILER("tcp_log_logs_to_buf")
            PROFILER("tcp_log_unref_bucket")
            PROFILER("tcp_flush_out_le")
            PROFILER("tcp_lro_rx2")
            PROFILER("tcp_lro_rx_csum_fixup")
            PROFILER("tcp_lro_sort")
            PROFILER("tcp_push_and_replace")
            PROFILER("tcp_reass_zone_change")
            PROFILER("tcp_sackhole_insert")
            PROFILER("tcp_default_fb_fini")
            PROFILER("tcp_default_fb_init")
            PROFILER("tcp_default_handoff_ok")
            PROFILER("tcp_destroy")
            PROFILER("tcp_getcred")
            PROFILER("tcp_inpcb_init")
            PROFILER("tcp_keyed_hash")
            PROFILER("tcp_log_addr")
            PROFILER("tcp_mtudisc")
            PROFILER("tcp_mtudisc_notify")
            PROFILER("tcp_notify")
            PROFILER("tcp_pcblist")
            PROFILER("tcp_tw_2msl_reset")
            PROFILER("tcp_twrespond")
            PROFILER("tcp_connect")
            PROFILER("tcp_disconnect")
            PROFILER("tcp_usr_abort")
            PROFILER("tcp_usr_accept")
            PROFILER("tcp_usr_attach")
            PROFILER("tcp_usr_bind")
            PROFILER("tcp_usr_close")
            PROFILER("tcp_usr_connect")
            PROFILER("tcp_usr_detach")
            PROFILER("tcp_usr_disconnect")
            PROFILER("tcp_usr_listen")
            PROFILER("tcp_usr_rcvd")
            PROFILER("tcp_usr_rcvoob")
            PROFILER("tcp_usr_ready")
            PROFILER("tcp_usr_send")
            PROFILER("tcp_usr_shutdown")
            PROFILER("tcp_usrclosed")
            PROFILER("tcp_log_dev_add_log")
            PROFILER("tcp_lro_flush_all")
            PROFILER("tcp_lro_free")
            PROFILER("tcp_lro_init_args")
            PROFILER("tcp_lro_queue_mbuf")
            PROFILER("tcp_ctlinput")
            PROFILER("tcp_ctloutput")
            PROFILER("tcp_drain")
            PROFILER("tcp_init")
            PROFILER("tcp_input")
            PROFILER("tcp_ccalgounload")
            PROFILER("tcp_compute_initwnd")
            PROFILER("tcp_compute_pipe")
            PROFILER("tcp_maxseg")
            PROFILER("tcp_fastopen_alloc_counter")
            PROFILER("tcp_fastopen_check_cookie")
            PROFILER("tcp_fastopen_connect")
            PROFILER("tcp_fastopen_decrement_counter")
            PROFILER("tcp_fastopen_disable_path")
            PROFILER("tcp_fastopen_init")
            PROFILER("tcp_fastopen_update_cache")
            PROFILER("tcp_mss")
            PROFILER("tcp_hc_get")
            PROFILER("tcp_hc_getmtu")
            PROFILER("tcp_hc_init")
            PROFILER("tcp_hc_update")
            PROFILER("tcp_hc_updatemtu")
            PROFILER("tcp_autorcvbuf")
            PROFILER("tcp_clean_sackreport")
            PROFILER("tcp_close")
            PROFILER("tcp_do_segment")
            PROFILER("tcp_dooptions")
            PROFILER("tcp_dropwithreset")
            PROFILER("tcp_handle_wakeup")
            PROFILER("tcp_log_addrs")
            PROFILER("tcp_log_event_")
            PROFILER("tcp_log_vain")
            PROFILER("tcp_maxmtu")
            PROFILER("tcp_maxmtu6")
            PROFILER("tcp_mss_update")
            PROFILER("tcp_mssopt")
            PROFILER("tcp_newreno_partial_ack")
            PROFILER("tcp_output")
            PROFILER("tcp_pulloutofband")
            PROFILER("tcp_reass")
            PROFILER("tcp_respond")
            PROFILER("tcp_sack_doack")
            PROFILER("tcp_state_change")
            PROFILER("tcp_timer_activate")
            PROFILER("tcp_timer_active")
            PROFILER("tcp_twcheck")
            PROFILER("tcp_twstart")
            PROFILER("tcp_update_dsack_list")
            PROFILER("tcp_update_sack_list")
            PROFILER("tcp_xmit_timer")
            PROFILER("tcp_log_drain")
            PROFILER("tcp_log_dump_tp_bucket_logbufs")
            PROFILER("tcp_log_dump_tp_logbuf")
            PROFILER("tcp_log_flowend")
            PROFILER("tcp_log_get_id")
            PROFILER("tcp_log_get_id_cnt")
            PROFILER("tcp_log_get_tag")
            PROFILER("tcp_log_getlogbuf")
            PROFILER("tcp_log_set_id")
            PROFILER("tcp_log_set_tag")
            PROFILER("tcp_log_state_change")
            PROFILER("tcp_log_tcpcbfini")
            PROFILER("tcp_log_tcpcbinit")
            PROFILER("tcp_lro_dereg_mbufq")
            PROFILER("tcp_lro_flush")
            PROFILER("tcp_lro_flush_inactive")
            PROFILER("tcp_lro_reg_mbufq")
            PROFILER("tcp_addoptions")
            PROFILER("tcp_clean_dsack_blocks")
            PROFILER("tcp_m_copym")
            PROFILER("tcp_offload_output")
            PROFILER("tcp_sack_adjust")
            PROFILER("tcp_sack_output")
            PROFILER("tcp_setpersist")
            PROFILER("tcp_sndbuf_autoscale")
            PROFILER("tcp_offload_connect")
            PROFILER("tcp_offload_listen_start")
            PROFILER("tcp_offload_listen_stop")
            PROFILER("tcp_reass_flush")
            PROFILER("tcp_free_sackholes")
            PROFILER("tcp_default_ctloutput")
            PROFILER("tcp_discardcb")
            PROFILER("tcp_drop_syn_sent")
            PROFILER("tcp_fini")
            PROFILER("tcp_inptoxtp")
            PROFILER("tcp_log_end_status")
            PROFILER("tcp_new_isn")
            PROFILER("tcp_new_ts_offset")
            PROFILER("tcp_newtcpcb")
            PROFILER("tcp_switch_back_to_default")
            PROFILER("tcp_timer_discard")
            PROFILER("tcp_timer_stop")
            PROFILER("tcp_tw_destroy")
            PROFILER("tcp_tw_zone_change")
            PROFILER("tcp_twclose")
            PROFILER("tcp_inpinfo_lock_del")
            PROFILER("tcp_timer_2msl")
            PROFILER("tcp_timer_delack")
            PROFILER("tcp_timer_keep")
            PROFILER("tcp_timer_persist")
            PROFILER("tcp_timer_rexmt")
            PROFILER("tcp_timers_unsuspend")
            PROFILER("tcp_tw_2msl_scan")
        }

        if (protocols.contains("tcp6")) {
            PROFILER("tcp6_getcred");
            PROFILER("tcp6_connect");
            PROFILER("tcp6_usr_accept");
            PROFILER("tcp6_usr_bind");
            PROFILER("tcp6_usr_connect");
            PROFILER("tcp6_usr_listen");
            PROFILER("tcp6_input");
            PROFILER("tcp6_ctlinput");
        }

        if (protocols.contains("udp")) {
            PROFILER("udp_abort");
            PROFILER("udp_append");
            PROFILER("udp_attach");
            PROFILER("udp_bind");
            PROFILER("udp_close");
            PROFILER("udp_common_ctlinput");
            PROFILER("udp_connect");
            PROFILER("udp_disconnect");
            PROFILER("udp_getcred");
            PROFILER("udp_inpcb_init");
            PROFILER("udp_pcblist");
            PROFILER("udp_send");
            PROFILER("udp_ctloutput");
            PROFILER("udp_input");
            PROFILER("udp_newudpcb");
            PROFILER("udp_notify");
            PROFILER("udp_set_kernel_tunneling");
            PROFILER("udp_shutdown");
        }

        if (protocols.contains("sctp")) {
            PROFILER("sctp_delayed_cksum");
            PROFILER("sctp_calculate_cksum");
            PROFILER("sctp_peeloff");
            PROFILER("sctp_generic_sendmsg");
            PROFILER("sctp_generic_sendmsg_iov");
            PROFILER("sctp_generic_recvmsg");
        }

        if (protocols.contains("udplite")) {
            PROFILER("udplite_inpcb_init");
            PROFILER("udplite_init");
        }

        if (protocols.contains("icmp")) {
            PROFILER("icmp_bandlimit_init");
            PROFILER("icmp_reflect");
            PROFILER("icmp_input");
            PROFILER("icmp_error");
        }

        if (protocols.contains("icmp6")) {
            PROFILER("icmp6_errcount");
            PROFILER("icmp6_reflect");
            PROFILER("icmp6_error");
            PROFILER("icmp6_ctloutput");
            PROFILER("icmp6_error2");
            PROFILER("icmp6_input");
            PROFILER("icmp6_mtudisc_update");
            PROFILER("icmp6_redirect_input");
            PROFILER("icmp6_redirect_output");
        }

        if (protocols.contains("igmp")) {
            PROFILER("igmp_intr");
            PROFILER("igmp_modevent");
            PROFILER("igmp_v1v2_queue_report");
            PROFILER("igmp_v3_cancel_link_timers");
            PROFILER("igmp_v3_enqueue_group_record");
            PROFILER("igmp_change_state");
            PROFILER("igmp_domifattach");
            PROFILER("igmp_domifdetach");
            PROFILER("igmp_fasttimo");
            PROFILER("igmp_ifdetach");
            PROFILER("igmp_input");
        }

        if (protocols.contains("tcp6")) {
            PROFILER("tcp6_getcred");
            PROFILER("tcp6_connect");
            PROFILER("tcp6_usr_accept");
            PROFILER("tcp6_usr_bind");
            PROFILER("tcp6_usr_connect");
            PROFILER("tcp6_usr_listen");
            PROFILER("tcp6_input");
            PROFILER("tcp6_ctlinput");
        }

        if (protocols.contains("udp6")) {
            PROFILER("udp6_abort");
            PROFILER("udp6_append");
            PROFILER("udp6_attach");
            PROFILER("udp6_bind");
            PROFILER("udp6_close");
            PROFILER("udp6_common_ctlinput");
            PROFILER("udp6_connect");
            PROFILER("udp6_disconnect");
            PROFILER("udp6_getcred");
            PROFILER("udp6_send");
            PROFILER("udp6_input");
        }

        dTrace.start();
    }

    void onTimer() override {
        dTrace.work(tmp_file);
    }

    ProfilerData onEnd() override {
        dTrace.stop();

        auto file = std::make_unique<QFile>();
        file->open(tmp_file, QIODevice::ReadOnly);
        file->reset();
        QTextStream stream(file.get());
        ProfilerParser parser(stream, 0);

        ProfilerData res;
        for (const auto pid: parser.getAvailablePids()) {
            for (const auto cpu: parser.getAvailableCPUs(pid)) {
                auto trees = parser.getProfilerTrees(cpu, pid);
                if (!trees.empty()) {
                    res[pid][cpu] << parser.getProfilerTrees(cpu, pid);
                }
            }
        }
        return res;
    }

    QStringList getSupportedProtocols() override {
        return {"ip", "ip6", "tcp", "tcp6", "udp", "udp6", "icmp", "icmp6", "sctp", "udplite", "igmp"};
    }

};

#undef PROFILER
#endif //LFNP_DTRACEPROFILERCOLLECTOR_H
