#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <net/udp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TheShenk");
MODULE_DESCRIPTION("Linux module for increment udp sockets drops stats");
MODULE_VERSION("0.0.1");

static int __init sockstatchanger_init(void) {
    
    struct sock *sk;
    pr_info("UDP drops increment init");
    
    for (int i=0; i<=udp_table.mask; i++) {
        
        struct udp_hslot *hslot = &udp_table.hash[i];
        if (hlist_empty(&hslot->head))
            continue;
        
        spin_lock_bh(&hslot->lock);
        sk_for_each(sk, &hslot->head) {
            atomic_inc(&sk->sk_drops);
        }
        spin_unlock_bh(&hslot->lock);
    }
    
    return 0;
}

static void __exit sockstatchanger_exit(void) {

    struct sock *sk;
    pr_info("UDP drops increment exit");

    for (int i=0; i<=udp_table.mask; i++) {

        struct udp_hslot *hslot = &udp_table.hash[i];
        if (hlist_empty(&hslot->head))
            continue;

        spin_lock_bh(&hslot->lock);
        sk_for_each(sk, &hslot->head) {
            atomic_dec(&sk->sk_drops);
        }
        spin_unlock_bh(&hslot->lock);
    }
}

module_init(sockstatchanger_init);
module_exit(sockstatchanger_exit);
