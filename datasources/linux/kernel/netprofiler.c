#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TheShenk");
MODULE_DESCRIPTION("Linux module for getting network functions execution time");
MODULE_VERSION("0.01");

struct exectime {
    ktime_t entry_stamp;
};

static int entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs) {
    struct exectime *data = (struct exectime *)ri->data;
    data->entry_stamp = ktime_get();
    return 0;    
}
NOKPROBE_SYMBOL(entry_handler);

static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    struct exectime *data = (struct exectime *)ri->data;
    ktime_t now = ktime_get();
    s64 delta = ktime_to_ns(ktime_sub(now, data->entry_stamp));
    
    pr_info("Took %lld ns to execute\n", (long long)delta);
    return 0;
}
NOKPROBE_SYMBOL(ret_handler);

static struct kretprobe netprofiler_kretprobe = {
    .handler = ret_handler,
    .entry_handler = entry_handler,
    .data_size = sizeof(struct exectime)
};

static int __init netprofiler_init(void) {
    int ret;

    netprofiler_kretprobe.kp.symbol_name = "udp_rcv";
    ret = register_kretprobe(&netprofiler_kretprobe);
    if (ret < 0) {
        pr_err("register_kretprobe failed, returned %d\n", ret);
        return ret;
    }
    pr_info("Planted return probe at %s: %p\n", netprofiler_kretprobe.kp.symbol_name, netprofiler_kretprobe.kp.addr);

    return 0;
}

static void __exit netprofiler_exit(void) {
    
    unregister_kretprobe(&netprofiler_kretprobe);
    pr_info("kretprobe at %p unregistered\n", netprofiler_kretprobe.kp.addr);

    /* nmissed > 0 suggests that maxactive was set too low. */
    pr_info("Missed probing %d instances of %s\n", netprofiler_kretprobe.nmissed, netprofiler_kretprobe.kp.symbol_name);
}

module_init(netprofiler_init);
module_exit(netprofiler_exit);
