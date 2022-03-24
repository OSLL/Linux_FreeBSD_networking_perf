#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include <linux/kprobes.h>

#include <linux/cdev.h>
#include <linux/fs.h>

#define DEVICE_NAME "netprofiler"
#define PROFILER_BUFFER_LEN 200

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TheShenk");
MODULE_DESCRIPTION("Linux module for getting network functions execution time");
MODULE_VERSION("0.0.1");

enum NodeType {
    
    UNINITIALIZED,
    ENTER,
    RETURN
    
};

struct profiler_node {
    enum NodeType type;
    const char *func_name;
    u64 time;
};

struct profiler_cpu {
    struct profiler_node list[PROFILER_BUFFER_LEN];
    int i;
};

static int device_open_count = 0;
static int major_num = 0;

static struct device *fw_device = NULL;
static struct class *fw_class = NULL;
static dev_t devt;

static DEFINE_PER_CPU(struct profiler_cpu, profiler_data);

/* Called when a process opens our device */
static int device_open(struct inode *inode, struct file *file) {
    /* If device is open, return busy */
    if (device_open_count) {
        return -EBUSY;
    }
    
    device_open_count++;
    return 0;
}

/* Called when a process closes our device */
static int device_release(struct inode *inode, struct file *file) {
    /* Decrement the open counter and usage count. Without this, the module would not unload. */
    device_open_count--;
    return 0;
}

#define LINE_LEN 255

static int write_profiler_string(struct profiler_node *node, char line[LINE_LEN], int cpu_index) {
    return snprintf(line, LINE_LEN, "%s %s %llu %d\n", 
            node->type == ENTER ? "enter" : "return", 
            node->func_name, node->time, cpu_index);
}


/* When a process reads from our device, this gets called. */
static ssize_t device_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {

    char result_line[LINE_LEN] = {0};
    int current_buffer_offset = 0;
    int cpu = 0;
    pr_info("Request: %d %d", len, *offset);
    if (!*offset) {
        
        for_each_possible_cpu(cpu) {
            struct profiler_cpu *cpu_profiler_data = per_cpu_ptr(&profiler_data, cpu);
            
            for (int i=0; i<PROFILER_BUFFER_LEN; i++) {

                int index = (cpu_profiler_data->i + i) % PROFILER_BUFFER_LEN;
                if (cpu_profiler_data->list[index].type == UNINITIALIZED) break;
                int write_count = write_profiler_string(&cpu_profiler_data->list[index], result_line, cpu);

                if (len > write_count) {
                
                    if (copy_to_user(buffer+current_buffer_offset, result_line, write_count)) {
                        return -EINVAL;
                    }
                    current_buffer_offset += write_count;
                    len -= write_count;
                    
                } else {
                    break;
                }
            }
        }

        *offset += current_buffer_offset;
        return current_buffer_offset;
    }
    return 0;
    
}

/* Called when a process tries to write to our device */
static ssize_t device_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    /* This is a read-only device */
    printk(KERN_ALERT "This operation is not supported.\n");
    return -EINVAL;
}

/* This structure points to all of the device functions */
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

struct exectime {
    ktime_t entry_stamp;
};

static int entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs) {
    
    struct profiler_cpu *cpu_profiler_data = NULL;        
    cpu_profiler_data = get_cpu_ptr(&profiler_data);
    
    cpu_profiler_data->list[cpu_profiler_data->i].type = ENTER;
    
    #if LINUX_VERSION_CODE <= KERNEL_VERSION(5,0,0)
        cpu_profiler_data->list[cpu_profiler_data->i].func_name = ri->rp->kp.symbol_name;
    #else
        cpu_profiler_data->list[cpu_profiler_data->i].func_name = ri->rph->rp->kp.symbol_name;
    #endif

    cpu_profiler_data->list[cpu_profiler_data->i].time = ktime_get_ns();
    
    cpu_profiler_data->i++;
    if (!(cpu_profiler_data->i < PROFILER_BUFFER_LEN)) {
        cpu_profiler_data->i = 0;
    }
    
    put_cpu_ptr(cpu_profiler_data);
    
    return 0;
}
NOKPROBE_SYMBOL(entry_handler);

static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs) {
    
    u64 now = ktime_get_ns();
    struct profiler_cpu *cpu_profiler_data = NULL;
    cpu_profiler_data = get_cpu_ptr(&profiler_data);
    
    cpu_profiler_data->list[cpu_profiler_data->i].type = RETURN;
    
    #if LINUX_VERSION_CODE <= KERNEL_VERSION(5,0,0)
        cpu_profiler_data->list[cpu_profiler_data->i].func_name = ri->rp->kp.symbol_name;
    #else
        cpu_profiler_data->list[cpu_profiler_data->i].func_name = ri->rph->rp->kp.symbol_name;
    #endif
        
    cpu_profiler_data->list[cpu_profiler_data->i].time = now;
    
    cpu_profiler_data->i++;
    if (!(cpu_profiler_data->i < PROFILER_BUFFER_LEN)) {
        cpu_profiler_data->i = 0;
    }
    
    put_cpu_ptr(cpu_profiler_data);
    
    return 0;
}
NOKPROBE_SYMBOL(ret_handler);

#define MAX_KPROBES_COUNT 511
#define KPROBES_LIST_OVERFLOW_ERR -255

static struct kretprobe netprofiler_kp_list[MAX_KPROBES_COUNT] = {};
static int netprofiler_kp_index = 0;

int add_profiler(char *func_name) {
    
    if (netprofiler_kp_index < MAX_KPROBES_COUNT) {
        netprofiler_kp_list[netprofiler_kp_index].handler = ret_handler;
        netprofiler_kp_list[netprofiler_kp_index].kp.addr = NULL;
        netprofiler_kp_list[netprofiler_kp_index].entry_handler = entry_handler;
        netprofiler_kp_list[netprofiler_kp_index].kp.symbol_name = func_name;
        
        int ret = register_kretprobe(&netprofiler_kp_list[netprofiler_kp_index]);
        if (!(ret < 0)) netprofiler_kp_index++;

        return ret;
    }
    return KPROBES_LIST_OVERFLOW_ERR;
}

#define PROFILER(func_name) do { \
    int ret = add_profiler(func_name); \
    if (ret < 0) pr_err(func_name " register_kretprobe failed, returned %d\n", ret); \
} while(0)

static int __init netprofiler_init(void) {
    
    int cpu = 0;
    for_each_possible_cpu(cpu) {
        struct profiler_cpu *cpu_profiler_data = per_cpu_ptr(&profiler_data, cpu);
        cpu_profiler_data->i = 0;
        
        for (int i=cpu_profiler_data->i; i<PROFILER_BUFFER_LEN; i++) {
            cpu_profiler_data->list[i].type = UNINITIALIZED;
        }
    }

    major_num = register_chrdev(0, "netprofiler", &file_ops);    
    fw_class = class_create(THIS_MODULE, DEVICE_NAME);
    
    devt = MKDEV(major_num, 0);
    fw_device = device_create(fw_class, NULL, devt, NULL, DEVICE_NAME);
    pr_info("Char device registered with number: %d, class: %p, device: %p\n", major_num, fw_class, fw_device);
    
    PROFILER("icmp_global_allow");
    PROFILER("igmp_mc_init");
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

    PROFILER("tcp_v4_rcv");
    PROFILER("tcp_v4_do_rcv");
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

    
    PROFILER("udp_rcv");
    PROFILER("udp_queue_rcv_skb");
    PROFILER("sock_queue_rcv");

    return 0;
}

static void __exit netprofiler_exit(void) {
    
    device_destroy(fw_class, devt);
    class_destroy(fw_class);
    unregister_chrdev(major_num, "netprofiler");
    
    for (int i=0; i<netprofiler_kp_index; i++) {
        unregister_kretprobe(&netprofiler_kp_list[i]);
        pr_info("Missed probing %d instances of %s\n", netprofiler_kp_list[i].nmissed, netprofiler_kp_list[i].kp.symbol_name);
        pr_info("kretprobe at %p unregistered\n", netprofiler_kp_list[i].kp.addr);
    }    
}

module_init(netprofiler_init);
module_exit(netprofiler_exit);
