#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Example output:
//
// $ dmesg
// [1] pid=1, execpath=/init
// [1] pid=2, execpath=sh
// [19] pid=3, execpath=ls
// [81] pid=4, execpath=cat
// [180] pid=5, execpath=echo
// [214] pid=6, execpath=echo
// [307] pid=7, execpath=ls
// [439] pid=8, execpath=grep
// [498] pid=10, execpath=ls
// [498] pid=11, execpath=grep


#define BUFSIZE 4096 * 4 + 1

int main() {
    char* buf = malloc(BUFSIZE); 
    if (!buf) {
        fprintf(2, "error: error occured during malloc()\n");
        exit(-2);
    }
    int ret = dmesg(buf);
    if (!ret) {
        fprintf(2, "error: error occured during dmesg(); error-code=%d\n", ret);
        free(buf);
        exit(-1);
    }

    printf("%s", buf);
    free(buf);
    exit(0);
}