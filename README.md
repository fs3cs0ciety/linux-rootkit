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
   * ^ Hides all the modules functions from `/sys/kernel/tracing/touched_functions` and `/proc/kallsyms`.
 *  


## Examples :

### Reseting the taint_mask to 0

<p align="center"><img src=".img/taint_mask.jpg"></p>

 * 

---
### Hiding `taint` from `/dev/kmsg`

<p align="center"><img src=".img/dmesg.jpg"></p>

 * 

---
### Hiding all of our functions from `/proc/kallsyms` and `/sys/kernel/tracing/touched_functions`

<p align="center"><img src=".img/hide.jpg"></p>

 * 

---
### Setting all ID's to 0

<p align="center"><img src=".img/root.jpg"></p>

 * 

---
### Adding/Removing the module from `/proc/modules` list

<p align="center"><img src=".img/lsmod.jpg"></p>

 * 

---
### Hiding Everything named `rasta`

<p align="center"><img src=".img/getdents64.jpg"></p>

 * In this image above we are using the `getdents64` hook to hide the directories and files named `rasta`. 

 * Technically we dont need to delete our module from the `/sys/module/` directory as it will not show up when running `ls -la /sys/module/ | grep rasta` because of the getdents hook we have in place. We see that the directory is still there but it's hidden from userspace and nobody is going to stumble upon rasta lol.

---
### Hiding Processes PID's 

<p align="center"><img src=".img/PID.jpg"></p>

 * Another use for the `getdents` and `kill` hook is hiding a userspace process pid. 

 * Currently having issues with hiding the process PID. just sends sigkill to the process still as of now. 
---
### Hiding Tcp connections on port 8081

<p align="center"><img src=".img/tcp.jpg"></p>

 * Rasta hooks two special `api's` called `tcp4_seq_show` and `tcp6_seq_show` using ftrace. 

---
### Reverse Shell 

<p align="center"><img src=".img/tcp.jpg"></p>

 * This is a super simple reverse shell that will listen for connections on the `localhost`. The localhost address can easily be replaced with whatever ip you give it in `kprobe/rev-shell.h`.

--- 

## Extra:
 > [!Important]
 > Not responsible for anything done with this. If your dumb enough to even think this is FUD malware go dude just go -_-. 

 ### Resources:
  
  * https://github.com/fs3cs0ciety/Linux-Rootkit-Series
  * https://github.com/matheuZSecurity/Rootkit
  * https://xcellerator.github.io/
  * https://github.com/torvalds/linux
  
 ### Arch Setup if wondering:
  * https://github.com/mylinuxforwork/dotfiles
