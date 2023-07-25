#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask); // 从系统调用中获得参数mask

  struct proc* p = myproc();
  p->mask = mask; // 将当前进程的mask值设为获取的参数
  
  return 0;
}

uint64
sys_info(void)
{
  // 用户空间指向sysinfo的指针
  uint64 u_addr;
  argaddr(0, &u_addr);

  struct sysinfo info;
  info.freemem = free_mem_num(); // 获取空闲内存
  info.nproc = proc_used_num();  // 获取非空闲进程

  struct proc *p = myproc();
  // 参考kernel/file.c/filestate()使用copy函数将info复制到用户空间
  if(copyout(p->pagetable, u_addr, (char*)(&info), sizeof(info)) < 0)
    return -1;

  return 0;
}