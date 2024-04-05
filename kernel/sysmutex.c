#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "proc.h"
#include "mutex.h"

struct mutex mtable[NMUTEX];
struct spinlock mtable_sp;

void mtable_init(void) {
    initlock(&mtable_sp, "");
    acquire(&mtable_sp);
    for (int i = 0; i < NMUTEX; ++i) {
        initsleeplock(&mtable[i].sl, "");
        mtable[i].proc_count = 0;
    }
    release(&mtable_sp);
}

int mutex_create(void) {
    acquire(&mtable_sp);
    struct proc* p = myproc();
    for (int i = 0; i < NMUTEX; ++i) {
        if (!mtable[i].proc_count) {
            ++mtable[i].proc_count;
            release(&mtable_sp);
            for (int j = 0; j < NOMUTEX; ++j) {
                if (p->omutex[j] == 0) {
                    p->omutex[j] = &mtable[i];
                    return j;
                }
            }
            return -2; // exceeded limit of open mutexes per process
        }
    }
    release(&mtable_sp);
    return -1; // all mutexes are used
}

int mutex_destroy(int md) {
    if (md < 0 || md >= NOMUTEX) {
        return -3; // mutex descriptor out of bounds
    }
    struct proc* p = myproc();
    if (p->omutex[md] == 0) {
        return -2; // unexisting mutex
    }
    acquire(&mtable_sp);
    --p->omutex[md]->proc_count;
    release(&mtable_sp);
    p->omutex[md] = 0;
    return 0;
}

int mutex_lock(int md) {
    if (md < 0 || md >= NOMUTEX) {
        return -3; // mutex descriptor out of bounds
    }
    struct proc* p = myproc();
    if (p->omutex[md] == 0) {
        return -2; // unexisting mutex
    }
    if (holdingsleep(&p->omutex[md]->sl)) {
        return -2; // double lock by one proc
    }
    acquiresleep(&p->omutex[md]->sl);
    return 0;
}

int mutex_unlock(int md) {
    if (md < 0 || md >= NOMUTEX) {
        return -3; // mutex descriptor out of bounds
    }
    struct proc* p = myproc();
    if (p->omutex[md] == 0) {
        return -2; // unexisting mutex
    }
    if (!holdingsleep(&p->omutex[md]->sl)) {
        return -2; // trying to unlock foreign mutex
    }
    releasesleep(&p->omutex[md]->sl);
    return 0;
}

// SYS CALLS

uint64 sys_mutex_create(void) {
    return mutex_create();
}

uint64 sys_mutex_destroy(void) {
    int md;
    argint(0, &md);
    return mutex_destroy(md);
}

uint64 sys_mutex_lock(void) {
    int md;
    argint(0, &md);
    return mutex_lock(md);
}

uint64 sys_mutex_unlock(void) {
    int md;
    argint(0, &md);
    return mutex_unlock(md);
}