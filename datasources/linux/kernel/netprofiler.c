#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/kprobes.h>

#include <linux/cdev.h>
#include <linux/fs.h>

#define DEVICE_NAME "netprofiler"
#define PROFILER_BUFFER_LEN 100

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TheShenk");
MODULE_DESCRIPTION("Linux module for getting network functions execution time");
MODULE_VERSION("0.01");

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
    if (!*offset) {
        pr_info("Read request: %lu %lld", len, *offset);
        
        for_each_possible_cpu(cpu) {
            struct profiler_cpu *cpu_profiler_data = per_cpu_ptr(&profiler_data, cpu);
            
            for (int i=cpu_profiler_data->i; i<PROFILER_BUFFER_LEN; i++) {
                
                if (cpu_profiler_data->list[i].type == UNINITIALIZED) break;
                int write_count = write_profiler_string(&cpu_profiler_data->list[i], result_line, cpu);
                cpu_profiler_data->list[i].type = UNINITIALIZED;
                
                if (copy_to_user(buffer+current_buffer_offset, result_line, write_count)) {
                    return -EINVAL;
                }
                current_buffer_offset += write_count;
            }
            
            for (int i=0; i<cpu_profiler_data->i; i++) {
                
                if (cpu_profiler_data->list[i].type == UNINITIALIZED) break;
                int write_count = write_profiler_string(&cpu_profiler_data->list[i], result_line, cpu);
                cpu_profiler_data->list[i].type = UNINITIALIZED;
                
                if (copy_to_user(buffer+current_buffer_offset, result_line, write_count)) {
                    return -EINVAL;
                }
                current_buffer_offset += write_count;
            }
            
            cpu_profiler_data->i = 0;
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
    cpu_profiler_data->list[cpu_profiler_data->i].func_name = ri->rph->rp->kp.symbol_name;
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
    cpu_profiler_data->list[cpu_profiler_data->i].func_name = ri->rph->rp->kp.symbol_name;
    cpu_profiler_data->list[cpu_profiler_data->i].time = now;
    
    cpu_profiler_data->i++;
    if (!(cpu_profiler_data->i < PROFILER_BUFFER_LEN)) {
        cpu_profiler_data->i = 0;
    }
    
    put_cpu_ptr(cpu_profiler_data);
    
    return 0;
}
NOKPROBE_SYMBOL(ret_handler);

#define MAX_KPROBES_COUNT 255
#define KPROBES_LIST_OVERFLOW_ERR -255

static struct kretprobe netprofiler_kp_list[MAX_KPROBES_COUNT] = {};
static int netprofiler_kp_index = 0;

int add_profiler(char *func_name) {
    
    if (netprofiler_kp_index < MAX_KPROBES_COUNT) {
        netprofiler_kp_list[netprofiler_kp_index].handler = ret_handler;
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
    
    PROFILER("ip_rcv");
    PROFILER("ip_rcv_core");
    PROFILER("ip_rcv_finish");
    PROFILER("ip_rcv_finish_core");
    PROFILER("ip_local_deliver");
    
    PROFILER("tcp_v4_rcv");
    
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
