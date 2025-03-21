#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_gethistory(void)
{
  struct proc_entry *temp_history;
  if (argptr(0, (void *)&temp_history, sizeof(struct proc_entry) * MAX_HISTORY) < 0)
    return -1;

  int history_size = 0, i = history_start;
  while(i != history_end){
    if(history[i].completed) {
      if(copyout(myproc()->pgdir, (uint)temp_history + history_size * sizeof(struct proc_entry), (char *)&history[i], sizeof(struct proc_entry)) < 0)
        return -1;

      history_size++;
    }

    i = (i + 1) % MAX_HISTORY;
  }

  return history_size;
}

int
sys_block(void)
{
  int syscall_id;
  if(argint(0, &syscall_id) < 0)
    return -1;

  if(syscall_id > 0 && syscall_id < MAX_SYSCALLS && is_blockable_syscall(syscall_id))
  {
    shells[myproc()->pid].blocked_syscalls[syscall_id] = 1;
    return 0;
  }

  return -1;
}

int
sys_unblock(void)
{
  int syscall_id;
  if(argint(0, &syscall_id) < 0)
    return -1;

  if(syscall_id > 0 && syscall_id < MAX_SYSCALLS && is_blockable_syscall(syscall_id))
  {
    if(shells[myproc()->pid].blocked_syscalls[syscall_id] == 0)
      return -1;
    
    shells[myproc()->pid].blocked_syscalls[syscall_id] = 0;
    return 0;
  }

  return -1;
}
