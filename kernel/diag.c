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

#define DMSIZE NPAGES * PAGESIZE

char dmesg_start[DMSIZE];
char* dmesg_pointer;
int dmesg_buffer_full;

struct {
    int locking;
    struct spinlock lock;
} dmesg_lock;

static void dmesg_putc(char c) {
    if (dmesg_pointer - dmesg_start >= DMSIZE) {
        dmesg_pointer = dmesg_start;
        dmesg_buffer_full = 1;
    }
    *(dmesg_pointer++) = c;
}

static char digits[] = "0123456789abcdef";

static void dmesg_printptr(uint64 x) {
    dmesg_putc('0');
    dmesg_putc('x');
    for (int i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
        dmesg_putc(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

static void dmesg_printint(int xx, int base, int sign) {
    char buf[16];
    int i;
    uint x;

    if (sign && (sign = xx < 0))
        x = -xx;
    else
        x = xx;

    i = 0;
    do {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);

    if (sign)
        buf[i++] = '-';

    while (--i >= 0)
        dmesg_putc(buf[i]);
}

static int print_dmesg_buffer(uint64 buf) {
    if (!buf)
        return -1; // nullptr buf

    if (dmesg_buffer_full) {
        // writing [current + 1, end] first
        if (copyout(myproc()->pagetable, 
                    buf, 
                    dmesg_pointer + 1,
                    sizeof(char) * (DMSIZE - (dmesg_pointer - dmesg_start) - 1)
                    ) < 0)
            return -2;
        buf += sizeof(char) * (DMSIZE - (dmesg_pointer - dmesg_start) - 1);
    }
    // writing [start, current]
    if (copyout(myproc()->pagetable, 
                buf, 
                dmesg_start,
                sizeof(char) * (dmesg_pointer - dmesg_start) + 1
                ) < 0)
        return -2;
    buf += sizeof(char) * (dmesg_pointer - dmesg_start) + 1;
    // add null-terminator at the end
    if (copyout(myproc()->pagetable, 
                buf, 
                "",
                sizeof(char) * 1
                ) < 0)
        return -2;
    buf += sizeof(char) * 1;

    return 1 + (dmesg_buffer_full ? DMSIZE : dmesg_pointer - dmesg_start);
}

void pr_msg(const char* fmt, ...) {
    va_list ap;
    int c;
    char* s;

    int locking = dmesg_lock.locking;
    if (locking)
        acquire(&dmesg_lock.lock);

    if (fmt == 0)
        panic("null fmt");

    uint xticks;
    acquire(&tickslock);
    xticks = ticks;
    release(&tickslock);

    dmesg_putc('[');
    dmesg_printint(xticks, 10, 1);
    dmesg_putc(']');
    dmesg_putc(' ');

    va_start(ap, fmt);
    for (int i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        if (c != '%') {
            dmesg_putc(c);
            continue;
        }
        c = fmt[++i] & 0xff;
        if (c == 0)
            break;
        switch (c) {
        case 'd':
            dmesg_printint(va_arg(ap, int), 10, 1);
            break;
        case 'x':
            dmesg_printint(va_arg(ap, int), 16, 1);
            break;
        case 'p':
            dmesg_printptr(va_arg(ap, uint64));
            break;
        case 's':
            if ((s = va_arg(ap, char*)) == 0)
                s = "(null)";
            for (; *s; s++)
                dmesg_putc(*s);
            break;
        case '%':
            dmesg_putc('%');
            break;
        default:
            // Print unknown % sequence to draw attention.
            dmesg_putc('%');
            dmesg_putc(c);
            break;
        }
    }
    va_end(ap);

    dmesg_putc('\n');

    if (locking)
        release(&dmesg_lock.lock);
}

void init_dmesg() {
    initlock(&dmesg_lock.lock, "pr");
    dmesg_lock.locking = 1;
    dmesg_pointer = dmesg_start;
    dmesg_buffer_full = 0;
}

// SYS CALL

uint64 sys_dmesg(void) {
    uint64 buf;
    argaddr(0, &buf);

    int locking = dmesg_lock.locking;
    if (locking)
        acquire(&dmesg_lock.lock);

    int ret = print_dmesg_buffer(buf);

    if (locking)
        release(&dmesg_lock.lock);

    return ret;
}