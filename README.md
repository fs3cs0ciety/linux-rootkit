# RastaMon Linux Rootkit

*Simple POC of a Linux kernel module posing as a rootkit to stealthily hook its syscalls and leave no trace!*

## Core Capabilities

Rasta has the following core capabilities:

* **Hide Files and Directories Named `rasta`**  
   Rasta uses the `getdents64` syscall hook to hide all files and directories named `rasta` from userspace. This is particularly useful for obfuscating the presence of the module in the filesystem.
  
* **Remove/Add Module from `/proc/modules` List**  
   The module can remove itself from the `/proc/modules` list, making it difficult for userspace tools like `lsmod` to detect its presence. It can also add itself back into the list when necessary.

* **Gain Root Capabilities**  
   By hooking the `hooked_kill` syscall, Rasta can set all process IDs (UIDs, GIDs, etc.) to 0, giving the module root privileges. This enables full control over the system without being detected.

* **Hide `taint` Messages from `dmesg` or `/dev/kmsg`**  
   Rasta hides any kernel `taint` messages that would typically show up in `dmesg` or `/dev/kmsg`. This helps to cover up any traces of kernel modifications, making the rootkit harder to detect.

* **Hide from `/sys/module` Directory**  
   The module hides itself from the `/sys/module` directory, preventing any attempts to manually detect or unload the module. This is achieved by manipulating the syscalls without actually deleting the module directory, making it invisible while still active.

* **Filter Module's Functions from `/sys/kernel/tracing/touched_functions` and `/proc/kallsyms`**  
   Rasta hooks `hooked_read` to filter out its own functions from being listed in `/sys/kernel/tracing/touched_functions` and `/proc/kallsyms`. This ensures that Rasta’s presence and behavior remain hidden in the kernel's symbol table and tracing logs.

* **Hiding Processes by PID**  
   Rasta hooks into the `getdents64` syscall to hide processes' PIDs, making it difficult for any monitoring tools to identify running processes associated with Rasta. This feature is still under development, but the goal is to ensure processes related to the rootkit remain stealthy.

* **Hiding Network Connections on Port 8081**  
   Rasta hooks two critical TCP APIs—`tcp4_seq_show` and `tcp6_seq_show`—using **ftrace** to hide any network connections on port 8081. This prevents tools like `netstat`, `lsof` and `ss` from displaying active connections on this port, ensuring covert communication for purposes like reverse shells.
   
* **Reverse Shell**  
   Rasta features a basic reverse shell that listens on a specified IP address (defaults to `localhost`). This shell can be configured to connect to an external IP address, providing a remote shell that remains hidden by the network connection.

* **Stealthy File System Operations**  
   The `getdents64` hook is also used to hide the presence of Rasta-related files and directories, ensuring that any references to the rootkit remain undetected by filesystem scans.

* **Hiding Module Presence in `lsmod` and Other Kernel Listings**  
   Rasta's stealth extends to hiding itself from `lsmod` and other kernel module listings, ensuring that the rootkit isn't visible in the list of loaded kernel modules.

* **Bypass Security Detection Tools**  
   All of these capabilities work together to make it extremely difficult for security detection tools and administrators to identify or remove Rasta. The rootkit operates entirely in memory, leaving minimal traces and ensuring that it remains undetected.

---

## Examples

### Resetting the taint_mask to 0

<p align="center"><img src=".img/taint_mask.jpg"></p>

* This demonstrates how Rasta manipulates the kernel's taint mask, effectively resetting it to 0.

---

### Hiding `taint` from `/dev/kmsg`

<p align="center"><img src=".img/dmesg.jpg"></p>

* Here, the `taint` message is hidden from `/dev/kmsg`, meaning any errors or warnings about the module loading won’t appear in the kernel log.

---

### Hiding All of Our Functions from `/proc/kallsyms` and `/sys/kernel/tracing/touched_functions`

<p align="center"><img src=".img/hide.jpg"></p>

* The `hooked_read` function is used to hide Rasta's functions from `/proc/kallsyms` and `/sys/kernel/tracing/touched_functions`. This ensures that the functions used by Rasta are never exposed to user space.

---

### Setting All IDs to 0

<p align="center"><img src=".img/root.jpg"></p>

* By using the `hooked_kill` syscall, Rasta sets all process IDs (UIDs, GIDs, etc.) to 0, effectively giving the kernel module root privileges.

---

### Adding/Removing the Module from the `/proc/modules` List

<p align="center"><img src=".img/lsmod.jpg"></p>

* This shows how Rasta can add or remove itself from the `/proc/modules` list. The module can be removed to hide its presence from userspace, while still being active in the kernel.

---

### Hiding Everything Named `rasta`

<p align="center"><img src=".img/getdents64.jpg"></p>

* The `getdents64` hook hides all files and directories named `rasta` from userspace. This is a stealth technique to ensure that the module’s presence is not detected through file listings or directory searches.

* **Note**: Technically, we don’t need to delete the module from `/sys/module/` because it won’t show up in directory listings due to the `getdents64` hook. While the directory is still there, it is hidden from userspace, making it hard to detect.

---

### Hiding Processes' PIDs

<p align="center"><img src=".img/PID.jpg"></p>

* The `getdents64` and `kill` hooks are also used to hide the process IDs (PIDs) of userspace processes. Currently, there are some issues with completely hiding PIDs, but SIGKILL is sent to the process.

---

### Hiding TCP Connections on Port 8081

<p align="center"><img src=".img/tcp.jpg"></p>

* One of the key features of Rasta is the ability to **hide network connections**. Specifically, we hook into two critical APIs: `tcp4_seq_show` and `tcp6_seq_show` using **ftrace**. These two functions are responsible for displaying information about IPv4 and IPv6 TCP connections in userspace.

* By intercepting these calls, Rasta can **hide** any active TCP connections on port 8081 from tools like `netstat` or `ss`. This ensures that network activity, such as reverse shells or other types of communication using port 8081, remains invisible to standard network monitoring tools.

* The core mechanism works by:
   1. **Hooking** the `tcp4_seq_show` and `tcp6_seq_show` functions.
   2. **Filtering out any connection** on port 8081 (or any other port you wish to target) from being listed.
   3. This method provides a stealthy way to maintain network communications without detection.

---

### Reverse Shell

<p align="center"><img src=".img/tcp.jpg"></p>

* This is a simple reverse shell that listens for connections on `localhost`. The IP address can be easily changed in `kprobe/rev-shell.h` to connect to an external host.
* The reverse shell utilizes the **hidden network connection** on port 8081, which is stealthed by Rasta's ftrace hook, allowing for covert communication.

---

## Extra

> [!Important]  
> Not responsible for anything done with this. If you're dumb enough to think this is FUD malware, just go away. 😑

### Resources:

* [Linux Rootkit Series by fs3cs0ciety](https://github.com/fs3cs0ciety/Linux-Rootkit-Series)
* [Rootkit by matheuZSecurity](https://github.com/matheuZSecurity/Rootkit)
* [Xcellerator's blog](https://xcellerator.github.io/)
* [The Linux Kernel Source](https://github.com/torvalds/linux)

### Arch Setup (if you're wondering):

* [Dotfiles by mylinuxforwork](https://github.com/mylinuxforwork/dotfiles)
