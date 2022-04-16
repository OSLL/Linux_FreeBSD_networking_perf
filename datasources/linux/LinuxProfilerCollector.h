//
// Created by shenk on 02.04.2022.
//

#ifndef LFNP_LINUXPROFILERCOLLECTOR_H
#define LFNP_LINUXPROFILERCOLLECTOR_H

#include <QDebug>

#include "../BaseProfilerCollector.h"

class LinuxProfilerCollector: public BaseProfilerCollector {

    ProfilerData res;
    quint64 time;

#define PROFILER(func_name) file.write(func_name); \
    file.flush()

    void onStart(QString protocol) override {
        qDebug() << "Protocol:" << protocol;

        time = 0;
        QFile file("/dev/netprofiler");
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Error: can't open profiler";
            return;
        }

        PROFILER("inet_current_timestamp");
        PROFILER("inet_get_local_port_range");
        PROFILER("ip4_datagram_connect");
        PROFILER("ip4_datagram_release_cb");
        PROFILER("ip_append_data");
        PROFILER("ip_append_page");
        PROFILER("ip_build_and_send_pkt");
        PROFILER("ip_call_ra_chain");
        PROFILER("ip_check_defrag");
        PROFILER("ip_cmsg_recv_offset");
        PROFILER("ip_cmsg_send");
        PROFILER("ip_defrag");
        PROFILER("ip_do_fragment");
        PROFILER("ip_fib_metrics_init");
        PROFILER("ip_flush_pending_frames");
        PROFILER("ip_forward");
        PROFILER("ip_forward_options");
        PROFILER("ip_frag_init");
        PROFILER("ip_frag_next");
        PROFILER("ip_fraglist_init");
        PROFILER("ip_fraglist_prepare");
        PROFILER("ip_generic_getfrag");
        PROFILER("ip_getsockopt");
        PROFILER("ip_icmp_error");
        PROFILER("ip_idents_reserve");
        PROFILER("ip_init");
        PROFILER("ip_list_rcv");
        PROFILER("ip_local_deliver");
        PROFILER("ip_local_error");
        PROFILER("ip_local_out");
        PROFILER("ip_make_skb");
        PROFILER("ip_mc_output");
        PROFILER("ip_misc_proc_init");
        PROFILER("ip_mr_input");
        PROFILER("ip_options_build");
        PROFILER("ip_options_compile");
        PROFILER("ip_options_fragment");
        PROFILER("ip_options_get");
        PROFILER("ip_options_rcv_srr");
        PROFILER("ip_options_undo");
        PROFILER("ip_output");
        PROFILER("ip_protocol_deliver_rcu");
        PROFILER("ip_push_pending_frames");
        PROFILER("ip_queue_xmit");
        PROFILER("ip_ra_control");
        PROFILER("ip_rcv");
        PROFILER("ip_recv_error");
        PROFILER("ip_send_check");
        PROFILER("ip_send_skb");
        PROFILER("ip_send_unicast_reply");
        PROFILER("ip_setsockopt");
        PROFILER("ip_sock_set_freebind");
        PROFILER("ip_sock_set_mtu_discover");
        PROFILER("ip_sock_set_pktinfo");
        PROFILER("ip_sock_set_recverr");
        PROFILER("ip_sock_set_tos");
        PROFILER("ip_static_sysctl_init");
        PROFILER("ipfrag_init");
        PROFILER("ipv4_pktinfo_prepare");
        PROFILER("__ip_append_data");
        PROFILER("__ip_finish_output");
        PROFILER("__ip_flush_pending_frames");
        PROFILER("__ip_local_out");
        PROFILER("__ip_make_skb");
        PROFILER("__ip_queue_xmit");
        PROFILER("csum_page");
        PROFILER("ip_copy_addrs");
        PROFILER("ip_copy_metadata");
        PROFILER("ip_cork_release");
        PROFILER("ip_finish_output");
        PROFILER("ip_finish_output2");
        PROFILER("ip_finish_output_gso");
        PROFILER("ip_frag_ipcb");
        PROFILER("ip_fragment");
        PROFILER("ip_mc_finish_output");
        PROFILER("ip_reply_glue_bits");
        PROFILER("ip_select_ttl");
        PROFILER("ip_setup_cork");

        if (protocol == "tcp") {
            PROFILER("tcp4_proc_exit");
            PROFILER("tcp4_proc_init");
            PROFILER("tcp_abort");
            PROFILER("tcp_add_backlog");
            PROFILER("tcp_alloc_md5sig_pool");
            PROFILER("tcp_assign_congestion_control");
            PROFILER("tcp_bpf_bypass_getsockopt");
            PROFILER("tcp_bpf_clone");
            PROFILER("tcp_bpf_get_proto");
            PROFILER("tcp_bpf_sendmsg_redir");
            PROFILER("tcp_bpf_update_proto");
            PROFILER("tcp_ca_find");
            PROFILER("tcp_ca_find_key");
            PROFILER("tcp_ca_get_key_by_name");
            PROFILER("tcp_ca_get_name_by_key");
            PROFILER("tcp_ca_openreq_child");
            PROFILER("tcp_check_oom");
            PROFILER("tcp_check_req");
            PROFILER("tcp_child_process");
            PROFILER("tcp_chrono_start");
            PROFILER("tcp_chrono_stop");
            PROFILER("tcp_clamp_probe0_to_user_timeout");
            PROFILER("tcp_cleanup_congestion_control");
            PROFILER("tcp_cleanup_rbuf");
            PROFILER("tcp_cleanup_ulp");
            PROFILER("tcp_clear_retrans");
            PROFILER("tcp_close");
            PROFILER("tcp_cong_avoid_ai");
            PROFILER("tcp_conn_request");
            PROFILER("tcp_connect");
            PROFILER("tcp_create_openreq_child");
            PROFILER("tcp_current_mss");
            PROFILER("tcp_cwnd_reduction");
            PROFILER("tcp_cwnd_restart");
            PROFILER("tcp_data_ready");
            PROFILER("tcp_delack_timer_handler");
            PROFILER("tcp_disconnect");
            PROFILER("tcp_done");
            PROFILER("tcp_enter_cwr");
            PROFILER("tcp_enter_loss");
            PROFILER("tcp_enter_memory_pressure");
            PROFILER("tcp_enter_quickack_mode");
            PROFILER("tcp_enter_recovery");
            PROFILER("tcp_fastopen_active_detect_blackhole");
            PROFILER("tcp_fastopen_active_disable");
            PROFILER("tcp_fastopen_active_disable_ofo_check");
            PROFILER("tcp_fastopen_active_should_disable");
            PROFILER("tcp_fastopen_add_skb");
            PROFILER("tcp_fastopen_cache_get");
            PROFILER("tcp_fastopen_cache_set");
            PROFILER("tcp_fastopen_cookie_check");
            PROFILER("tcp_fastopen_ctx_destroy");
            PROFILER("tcp_fastopen_defer_connect");
            PROFILER("tcp_fastopen_destroy_cipher");
            PROFILER("tcp_fastopen_get_cipher");
            PROFILER("tcp_fastopen_init_key_once");
            PROFILER("tcp_fastopen_reset_cipher");
            PROFILER("tcp_filter");
            PROFILER("tcp_fin");
            PROFILER("tcp_finish_connect");
            PROFILER("tcp_fragment");
            PROFILER("tcp_free_fastopen_req");
            PROFILER("tcp_get_allowed_congestion_control");
            PROFILER("tcp_get_available_congestion_control");
            PROFILER("tcp_get_available_ulp");
            PROFILER("tcp_get_cookie_sock");
            PROFILER("tcp_get_default_congestion_control");
            PROFILER("tcp_get_info");
            PROFILER("tcp_get_md5sig_pool");
            PROFILER("tcp_get_syncookie_mss");
            PROFILER("tcp_getsockopt");
            PROFILER("tcp_gro_complete");
            PROFILER("tcp_gro_receive");
            PROFILER("tcp_gso_segment");
            PROFILER("tcp_init");
            PROFILER("tcp_init_congestion_control");
            PROFILER("tcp_init_cwnd");
            PROFILER("tcp_init_metrics");
            PROFILER("tcp_init_sock");
            PROFILER("tcp_init_transfer");
            PROFILER("tcp_init_xmit_timers");
            PROFILER("tcp_initialize_rcv_mss");
            PROFILER("tcp_ioctl");
            PROFILER("tcp_ld_RTO_revert");
            PROFILER("tcp_leave_memory_pressure");
            PROFILER("tcp_make_synack");
            PROFILER("tcp_mark_push");
            PROFILER("tcp_mark_skb_lost");
            PROFILER("tcp_md5_do_add");
            PROFILER("tcp_md5_do_del");
            PROFILER("tcp_md5_hash_key");
            PROFILER("tcp_md5_hash_skb_data");
            PROFILER("tcp_metrics_init");
            PROFILER("tcp_mmap");
            PROFILER("tcp_mss_to_mtu");
            PROFILER("tcp_mstamp_refresh");
            PROFILER("tcp_mtu_to_mss");
            PROFILER("tcp_mtup_init");
            PROFILER("tcp_newreno_mark_lost");
            PROFILER("tcp_oow_rate_limited");
            PROFILER("tcp_openreq_init_rwin");
            PROFILER("tcp_orphan_count_sum");
            PROFILER("tcp_pace_kick");
            PROFILER("tcp_parse_md5sig_option");
            PROFILER("tcp_parse_options");
            PROFILER("tcp_peek_len");
            PROFILER("tcp_peer_is_proven");
            PROFILER("tcp_poll");
            PROFILER("tcp_push");
            PROFILER("tcp_push_one");
            PROFILER("tcp_rack_advance");
            PROFILER("tcp_rack_mark_lost");
            PROFILER("tcp_rack_reo_timeout");
            PROFILER("tcp_rack_skb_timeout");
            PROFILER("tcp_rack_update_reo_wnd");
            PROFILER("tcp_rate_check_app_limited");
            PROFILER("tcp_rate_gen");
            PROFILER("tcp_rate_skb_delivered");
            PROFILER("tcp_rate_skb_sent");
            PROFILER("tcp_rbtree_insert");
            PROFILER("tcp_rcv_established");
            PROFILER("tcp_rcv_space_adjust");
            PROFILER("tcp_rcv_state_process");
            PROFILER("tcp_read_sock");
            PROFILER("tcp_rearm_rto");
            PROFILER("tcp_recv_timestamp");
            PROFILER("tcp_recvmsg");
            PROFILER("tcp_register_congestion_control");
            PROFILER("tcp_register_ulp");
            PROFILER("tcp_release_cb");
            PROFILER("tcp_remove_empty_skb");
            PROFILER("tcp_reno_cong_avoid");
            PROFILER("tcp_reno_ssthresh");
            PROFILER("tcp_reno_undo_cwnd");
            PROFILER("tcp_req_err");
            PROFILER("tcp_reset");
            PROFILER("tcp_retransmit_skb");
            PROFILER("tcp_retransmit_timer");
            PROFILER("tcp_rtx_synack");
            PROFILER("tcp_schedule_loss_probe");
            PROFILER("tcp_select_initial_window");
            PROFILER("tcp_send_ack");
            PROFILER("tcp_send_active_reset");
            PROFILER("tcp_send_delayed_ack");
            PROFILER("tcp_send_fin");
            PROFILER("tcp_send_loss_probe");
            PROFILER("tcp_send_mss");
            PROFILER("tcp_send_partial");
            PROFILER("tcp_send_probe0");
            PROFILER("tcp_send_rcvq");
            PROFILER("tcp_send_synack");
            PROFILER("tcp_send_window_probe");
            PROFILER("tcp_sendmsg");
            PROFILER("tcp_sendmsg_locked");
            PROFILER("tcp_sendpage");
            PROFILER("tcp_sendpage_locked");
            PROFILER("tcp_seq_next");
            PROFILER("tcp_seq_start");
            PROFILER("tcp_seq_stop");
            PROFILER("tcp_set_allowed_congestion_control");
            PROFILER("tcp_set_congestion_control");
            PROFILER("tcp_set_default_congestion_control");
            PROFILER("tcp_set_keepalive");
            PROFILER("tcp_set_rcvlowat");
            PROFILER("tcp_set_state");
            PROFILER("tcp_set_ulp");
            PROFILER("tcp_set_window_clamp");
            PROFILER("tcp_setsockopt");
            PROFILER("tcp_shutdown");
            PROFILER("tcp_simple_retransmit");
            PROFILER("tcp_skb_collapse_tstamp");
            PROFILER("tcp_skb_entail");
            PROFILER("tcp_skb_mark_lost_uncond_verify");
            PROFILER("tcp_slow_start");
            PROFILER("tcp_splice_read");
            PROFILER("tcp_stream_alloc_skb");
            PROFILER("tcp_stream_memory_free");
            PROFILER("tcp_syn_ack_timeout");
            PROFILER("tcp_synack_rtt_meas");
            PROFILER("tcp_sync_mss");
            PROFILER("tcp_tasklet_init");
            PROFILER("tcp_time_wait");
            PROFILER("tcp_timewait_state_process");
            PROFILER("tcp_trim_head");
            PROFILER("tcp_try_fastopen");
            PROFILER("tcp_twsk_destructor");
            PROFILER("tcp_twsk_unique");
            PROFILER("tcp_unregister_congestion_control");
            PROFILER("tcp_unregister_ulp");
            PROFILER("tcp_update_metrics");
            PROFILER("tcp_update_recv_tstamps");
            PROFILER("tcp_update_ulp");
            PROFILER("tcp_v4_conn_request");
            PROFILER("tcp_v4_connect");
            PROFILER("tcp_v4_destroy_sock");
            PROFILER("tcp_v4_do_rcv");
            PROFILER("tcp_v4_early_demux");
            PROFILER("tcp_v4_err");
            PROFILER("tcp_v4_get_syncookie");
            PROFILER("tcp_v4_init");
            PROFILER("tcp_v4_md5_hash_skb");
            PROFILER("tcp_v4_md5_lookup");
            PROFILER("tcp_v4_mtu_reduced");
            PROFILER("tcp_v4_rcv");
            PROFILER("tcp_v4_send_check");
            PROFILER("tcp_v4_syn_recv_sock");
            PROFILER("tcp_v4_tw_remember_stamp");
            PROFILER("tcp_v6_get_syncookie");
            PROFILER("tcp_wfree");
            PROFILER("tcp_write_queue_purge");
            PROFILER("tcp_write_timer_handler");
            PROFILER("tcp_write_wakeup");
            PROFILER("tcp_xmit_retransmit_queue");
            PROFILER("tcpv4_offload_init");

            PROFILER("__pskb_trim_head");
            PROFILER("__tcp_mtu_to_mss");
            PROFILER("__tcp_push_pending_frames");
            PROFILER("__tcp_retransmit_skb");
            PROFILER("__tcp_select_window");
            PROFILER("__tcp_send_ack");
            PROFILER("__tcp_transmit_skb");
            PROFILER("bpf_skops_hdr_opt_len");
            PROFILER("bpf_skops_write_hdr_opt");
            PROFILER("bpf_skops_write_hdr_opt_arg0");
            PROFILER("mptcp_options_write");
            PROFILER("mptcp_set_option_cond");
            PROFILER("sk_forced_mem_schedule");
            PROFILER("skb_still_in_host_queue");
            PROFILER("smc_options_write");
            PROFILER("smc_set_option");
            PROFILER("smc_set_option_cond");
            PROFILER("tcp_acceptable_seq");
            PROFILER("tcp_adjust_pcount");
            PROFILER("tcp_advertise_mss");
            PROFILER("tcp_ca_dst_init");
            PROFILER("tcp_can_coalesce_send_queue_head");
            PROFILER("tcp_can_collapse");
            PROFILER("tcp_chrono_set");
            PROFILER("tcp_collapse_retrans");
            PROFILER("tcp_connect_init");
            PROFILER("tcp_connect_queue_skb");
            PROFILER("tcp_cwnd_application_limited");
            PROFILER("tcp_cwnd_test");
            PROFILER("tcp_cwnd_validate");
            PROFILER("tcp_ecn_clear_syn");
            PROFILER("tcp_ecn_make_synack");
            PROFILER("tcp_ecn_send");
            PROFILER("tcp_ecn_send_syn");
            PROFILER("tcp_ecn_send_synack");
            PROFILER("tcp_established_options");
            PROFILER("tcp_event_ack_sent");
            PROFILER("tcp_event_data_sent");
            PROFILER("tcp_event_new_data_sent");
            PROFILER("tcp_fragment_tstamp");
            PROFILER("tcp_has_tx_tstamp");
            PROFILER("tcp_init_nondata_skb");
            PROFILER("tcp_init_tso_segs");
            PROFILER("tcp_insert_write_queue_after");
            PROFILER("tcp_minshall_check");
            PROFILER("tcp_minshall_update");
            PROFILER("tcp_mss_split_point");
            PROFILER("tcp_mtu_check_reprobe");
            PROFILER("tcp_mtu_probe");
            PROFILER("tcp_nagle_check");
            PROFILER("tcp_nagle_test");
            PROFILER("tcp_pacing_check");
            PROFILER("tcp_queue_skb");
            PROFILER("tcp_retrans_try_collapse");
            PROFILER("tcp_send_syn_data");
            PROFILER("tcp_set_skb_tso_segs");
            PROFILER("tcp_skb_fragment_eor");
            PROFILER("tcp_small_queue_check");
            PROFILER("tcp_snd_wnd_test");
            PROFILER("tcp_syn_options");
            PROFILER("tcp_synack_options");
            PROFILER("tcp_tasklet_func");
            PROFILER("tcp_tso_autosize");
            PROFILER("tcp_tso_segs");
            PROFILER("tcp_tso_should_defer");
            PROFILER("tcp_tsq_handler");
            PROFILER("tcp_tsq_write");
            PROFILER("tcp_update_skb_after_send");
            PROFILER("tcp_urg_mode");
            PROFILER("tcp_write_xmit");
            PROFILER("tcp_xmit_probe_skb");
            PROFILER("tso_fragment");
        }

        if (protocol == "udp") {
            PROFILER("__first_packet_length");
            PROFILER("__skb_recv_udp");
            PROFILER("__udp4_lib_demux_lookup");
            PROFILER("__udp4_lib_err");
            PROFILER("__udp4_lib_err_encap");
            PROFILER("__udp4_lib_err_encap_no_sk");
            PROFILER("__udp4_lib_lookup");
            PROFILER("__udp4_lib_lookup_skb");
            PROFILER("__udp4_lib_mcast_deliver");
            PROFILER("__udp4_lib_mcast_demux_lookup");
            PROFILER("__udp4_lib_rcv");
            PROFILER("__udp_cmsg_send");
            PROFILER("__udp_disconnect");
            PROFILER("__udp_enqueue_schedule_skb");
            PROFILER("__udp_is_mcast_sock");
            PROFILER("__udp_queue_rcv_skb");
            PROFILER("__udp_sysctl_init");
            PROFILER("bpf_iter_fini_udp");
            PROFILER("bpf_iter_register");
            PROFILER("bpf_iter_udp_seq_show");
            PROFILER("bpf_iter_udp_seq_stop");
            PROFILER("busylock_acquire");
            PROFILER("busylock_release");
            PROFILER("compute_score");
            PROFILER("first_packet_length");
            PROFILER("lookup_reuseport");
            PROFILER("set_uhash_entries");
            PROFILER("skb_consume_udp");
            PROFILER("udp4_csum_init");
            PROFILER("udp4_format_sock");
            PROFILER("udp4_hwcsum");
            PROFILER("udp4_lib_lookup");
            PROFILER("udp4_lib_lookup2");
            PROFILER("udp4_lib_lookup_skb");
            PROFILER("udp4_lookup_run_bpf");
            PROFILER("udp4_proc_exit");
            PROFILER("udp4_proc_exit_net");
            PROFILER("udp4_proc_init");
            PROFILER("udp4_proc_init_net");
            PROFILER("udp4_seq_show");
            PROFILER("udp_abort");
            PROFILER("udp_cmsg_send");
            PROFILER("udp_destroy_sock");
            PROFILER("udp_destruct_sock");
            PROFILER("udp_disconnect");
            PROFILER("udp_ehashfn");
            PROFILER("udp_encap_disable");
            PROFILER("udp_encap_enable");
            PROFILER("udp_err");
            PROFILER("udp_flow_hashrnd");
            PROFILER("udp_flush_pending_frames");
            PROFILER("udp_get_first");
            PROFILER("udp_get_idx");
            PROFILER("udp_get_next");
            PROFILER("udp_getsockopt");
            PROFILER("udp_init");
            PROFILER("udp_init_sock");
            PROFILER("udp_ioctl");
            PROFILER("udp_lib_get_port");
            PROFILER("udp_lib_getsockopt");
            PROFILER("udp_lib_lport_inuse");
            PROFILER("udp_lib_lport_inuse2");
            PROFILER("udp_lib_rehash");
            PROFILER("udp_lib_setsockopt");
            PROFILER("udp_lib_unhash");
            PROFILER("udp_poll");
            PROFILER("udp_pre_connect");
            PROFILER("udp_prog_seq_show");
            PROFILER("udp_push_pending_frames");
            PROFILER("udp_queue_rcv_one_skb");
            PROFILER("udp_queue_rcv_skb");
            PROFILER("udp_rcv");
            PROFILER("udp_read_sock");
            PROFILER("udp_recvmsg");
            PROFILER("udp_reuseport_add_sock");
            PROFILER("udp_rmem_release");
            PROFILER("udp_send_skb");
            PROFILER("udp_sendmsg");
            PROFILER("udp_sendpage");
            PROFILER("udp_seq_next");
            PROFILER("udp_seq_start");
            PROFILER("udp_seq_stop");
            PROFILER("udp_set_csum");
            PROFILER("udp_set_dev_scratch");
            PROFILER("udp_setsockopt");
            PROFILER("udp_sk_rx_dst_set");
            PROFILER("udp_skb_csum_unnecessary_set");
            PROFILER("udp_skb_destructor");
            PROFILER("udp_skb_dtor_locked");
            PROFILER("udp_skb_has_head_state");
            PROFILER("udp_skb_truesize");
            PROFILER("udp_sysctl_init");
            PROFILER("udp_table_init");
            PROFILER("udp_try_make_stateless");
            PROFILER("udp_unicast_rcv_skb");
            PROFILER("udp_v4_early_demux");
            PROFILER("udp_v4_get_port");
            PROFILER("udp_v4_rehash");
        }

        file.close();
    }
#undef PROFILER

    void onTimer() override {
        ProfilerParser parser("/dev/netprofiler", time);
        time = parser.getLastTokenTimestamp();
        for (const auto pid: parser.getAvailablePids()) {
            for (const auto cpu: parser.getAvailableCPUs(pid)) {
                auto trees = parser.getProfilerTrees(cpu, pid);
                if (!trees.empty()) {
                    res[pid][cpu] << parser.getProfilerTrees(cpu, pid);
                }
            }
        }
    }

    ProfilerData onEnd() override {

        QFile file("/dev/netprofiler");
        if (file.open(QIODevice::WriteOnly)) {
            file.write("clean");
            file.close();
        }

        return res;
    }

    QStringList getSupportedProtocols() override {
        return {"tcp", "udp"};
    }

};


#endif //LFNP_LINUXPROFILERCOLLECTOR_H
