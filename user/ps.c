#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const char* sUNUSED      = "unused";
const char* sUSED        = "used";
const char* sSLEEPING    = "sleep";
const char* sRUNNABLE    = "runnable";
const char* sRUNNING     = "running";
const char* sZOMBIE      = "zombie";
const char* sUNKNOWN     = "unknown";

const char* state_to_str(enum procstate state) {
    if (state == UNUSED) {
        return sUNUSED;
    } else if (state == USED) {
        return sUSED;
    } else if (state == SLEEPING) {
        return sSLEEPING;
    } else if (state == RUNNABLE) {
        return sRUNNABLE;
    } else if (state == RUNNING) {
        return sRUNNING;
    } else if (state == RUNNABLE) {
        return sRUNNABLE;
    } else {
        return sUNKNOWN;
    }
}

int main() {

    int bufsize = 8, lim;
    struct procinfo *plist;
    while (1) { 
        plist = malloc(sizeof(struct procinfo) * bufsize);
        if (plist == 0) {
            printf("ps failed! Insufficient memory for buffer\n");
            exit(1);
        }
        lim = ps_listinfo(plist, bufsize);
        if (lim == -1) {
            free(plist);
            bufsize *= 2;
            continue;
        } else if (lim == -2) {
            free(plist);
            printf("ps failed! Invalid user-space address\n", lim);
            exit(2);
        } else {
            break;
        }
    }

    printf("PARENT\tSTATE\t\tNAME\n");

    struct procinfo* p;
    for (int i = 0; i < lim; ++i) {
        p = plist + i;
        printf("%d\t%s\t\t%s\n", p->parent_pid, state_to_str(p->state), p->name);
    }

    exit(0);
}