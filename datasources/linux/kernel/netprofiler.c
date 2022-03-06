#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/kprobes.h>

#include <linux/cdev.h>
#include <linux/fs.h>

#define DEVICE_NAME "netprofiler"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TheShenk");
MODULE_DESCRIPTION("Linux module for getting network functions execution time");
MODULE_VERSION("0.01");

static int device_open_count = 0;
static int major_num = 0;

static struct device *fw_device = NULL;
static struct class *fw_class = NULL;
static dev_t devt;

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

/* When a process reads from our device, this gets called. */
static ssize_t device_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {

    char str[42] = {0};
    int i = 42;
    int slen = 0;
    
    snprintf(str, 42, "%d", i);
    slen = strlen(str);
    pr_info("Read: %d", slen);
    
    if (copy_to_user(buffer, str, 42))
        return -EINVAL;
    
    *offset = slen+1;
    
    return slen;
    
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
    struct exectime *data = (struct exectime *)ri->data;
    data->entry_stamp = ktime_get();
    return 0;
}
NOKPROBE_SYMBOL(entry_handler);

static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs) {
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

    int ret = 0;

    major_num = register_chrdev(0, "netprofiler", &file_ops);    
    fw_class = class_create(THIS_MODULE, DEVICE_NAME);
    
    devt = MKDEV(major_num, 0);
    fw_device = device_create(fw_class, NULL, devt, NULL, DEVICE_NAME);
    pr_info("Char device registered with number: %d, class: %p, device: %p\n", major_num, fw_class, fw_device);
    
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
    
    device_destroy(fw_class, devt);
    class_destroy(fw_class);
    unregister_chrdev(major_num, "netprofiler");
    
    unregister_kretprobe(&netprofiler_kretprobe);
    
    pr_info("kretprobe at %p unregistered\n", netprofiler_kretprobe.kp.addr);

    /* nmissed > 0 suggests that maxactive was set too low. */
    pr_info("Missed probing %d instances of %s\n", netprofiler_kretprobe.nmissed, netprofiler_kretprobe.kp.symbol_name);
}

module_init(netprofiler_init);
module_exit(netprofiler_exit);
