#include <stdarg.h>
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"
#include "file.h"

#define NMODES 4
#define MAXUNSIGNED 0xFFFFFFFFU

struct {
    struct spinlock lock;
    uint mode[NMODES];
    // Modes:
    // 1) syscalls
    // 2) dev. interrupts
    // 3) proc switches
    // 4) exec calls
} protocolist;

static void set_modes(int modes, int rticks) {
    // `modes` in format 0bABCD, where D - 1st mode, C - 2nd, ...
    // `rticks`: positive for timeout set, zero for no timeout, negative for disable
    // 
    // Examples:
    // modes=0b1111, rticks=-1      -- enable all modes, no timeout 
    // modes=0b1000, rticks=1000    -- enable 4th mode, timeout after 1000 ticks
    // modes=0b0011, rticks=0       -- disable 1nd and 2rd modes

    if (rticks > 0) {
        // enable with timeout
        
        acquire(&tickslock);
        uint xticks = ticks;
        release(&tickslock);

        xticks += rticks;
        // unsigned int overflow check
        rticks = (xticks < rticks) ? MAXUNSIGNED : xticks;
    } else if (rticks == 0) {
        // enable, no timeout
        
        rticks = MAXUNSIGNED;
    } else {
        // disable modes
        
        rticks = 0;
    }

    acquire(&protocolist.lock);
    for (int m = 1, i = 0; i < NMODES; m <<= 1, ++i)
        if (modes & m)
            protocolist.mode[i] = rticks;
    release(&protocolist.lock);
}

void protocol_log(int mode, const char* fmt, ...) {
    // mode counted from 1

    acquire(&tickslock);
    uint xticks = ticks;
    release(&tickslock);

    if (protocolist.mode[mode - 1] > xticks) {
        va_list args;
        va_start(args, fmt);
        pr_msg(fmt, args);
        va_end(args);
    }
}

void init_protocol(void) {
    initlock(&protocolist.lock, "protocol.c lock");
    for (int i = 0; i < NMODES; ++i)
        protocolist.mode[i] = 0;
}

// SYSCALLS

uint64 sys_prset(void) {
    int modes, rticks;
    argint(0, &modes);
    argint(1, &rticks);
    set_modes(modes, rticks);
    return 0;
}