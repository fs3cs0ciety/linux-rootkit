# RastaMon linux-rootkit
* simple POC of a linux kernel module posing as a rootkit to stealthly hooks its syscalls and leave no trace!

## Core Capablities
Rasta has the following capablities below :

 * Hide every file/directory with the name `rasta` from userspace with a getdents hook.
 * Remove/Add the module from the `/proc/modules` list.
 * Get root capablities by using the `hooked_kill` syscall and setting all ID's to 0.
 * Hides `taint` message from `dmesg` or `/dev/kmsg`.
 * Hides from `/sys/module` without deleting the module from the directory. More explained on this below, followed by examples.
 * `hooked_read` will filter out all lines with the word `rasta` for example because this is the modules name that we want to hide.
  * ^ Hides all modules functions from `/sys/kernel/tracing/touched_functions`, `/proc/kallsyms`, and also  
