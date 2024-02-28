#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"

uint64 sys_add(void) {

    uint64 dst;
    argaddr(0, &dst);

    int x, y;
    argint(1, &x);
    argint(2, &y);

    int res = x + y;

    int ret = copyout(myproc()->pagetable, dst, (char*)&res, sizeof(int));
    if (ret < 0) return -3;

    return 0;
}