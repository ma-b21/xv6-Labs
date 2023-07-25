#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 base;        // 待检查页面起始地址
  int pagenum;        // 待检查页面数
  uint64 usraddr;     // 等待输出结果的用户空间地址

  // 获取参数
  argaddr(0, &base);
  argint(1, &pagenum);
  argaddr(2, &usraddr);

  // bitmask
  uint64 maskbits = 0;
  struct proc* p = myproc();

  for(int i = 0; i < pagenum; ++i)
  {
    pte_t* pte = walk(p->pagetable, base + i * PGSIZE, 0);  // 获取第i页的第0个页表项映射的物理地址
    if(pte == 0)  panic("page not exist!");
    if(PTE_FLAGS(*pte) & PTE_A)  // 访问位为1，代表被访问过
    {
      maskbits |= (1L << i);
    }
    *pte = ((*pte & PTE_A) ^ *pte) ^ 0;  // 将PTE_A置零
  }
  
  if (copyout(p->pagetable, usraddr, (char *)&maskbits, sizeof(maskbits)) < 0) // 将bitmask的结果输出到用户空间
    panic("sys_pgacess copyout error");
  
  return 0;
}
#endif

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
