#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
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


// writes bytes 0 or 1 into dest array:
//   0 - page hadn't been accessed since last pgaccess() call
//   1 - page had been accessed
// dest size should be [pages + 1], since memory block of size 
// (pages * PGSIZE) bytes possibly can intersect pages boundaries
uint64 sys_pgaccess(void) {
  uint64 paddr, dest;
  int pages;

  argaddr(0, &paddr);
  argint(1, &pages);
  argaddr(2, &dest);

  pagetable_t pagetable = myproc()->pagetable;

  uint64 end_addr = paddr + pages * PGSIZE + 1;
  uint64 addr = paddr - paddr % PGSIZE;

  char accessed[pages];
  for (int i = 0; addr < end_addr; addr += PGSIZE, ++i) {
    pte_t *pte = walk(pagetable, addr, 0);
    if (*pte & PTE_A) {
      accessed[i] = 1;
      
      // clear PTE_A bits after checking
      *pte &= ~PTE_A;
    } else {
      accessed[i] = 0;
    }
  }

  if (copyout(pagetable, dest, accessed, sizeof(accessed)) < 0) {
    // copyout failure
    return 1;
  }
  
  return 0;
}