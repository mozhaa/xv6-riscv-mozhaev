#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void test1() {
    // invalid address
    int x;
    int ret = ps_listinfo((struct procinfo*)(&x + 0xFABCDE), 16); 
    if (ret == -2) {
        printf("Test #1 (invalid address test) success!\n");
    } else {
        printf("Test #1 (invalid address test) failed! Return code: %d\n", ret);
    }
}

void test2() {
    // insufficient buffer size
    struct procinfo plist[16];
    int ret = ps_listinfo(plist, 1);
    if (ret == -1) {
        printf("Test #2 (insufficient buffer size test) success!\n");
    } else {
        printf("Test #2 (insufficient buffer size test) failed! Return code: %d\n", ret);
    }
}

void test3() {
    // success run
    int bufsize = 8;
    struct procinfo *plist;
    while (1) { 
        plist = malloc(sizeof(struct procinfo) * bufsize);
        if (plist == 0) {
            printf("Test #3 (success run test) failed! Insufficient memory for buffer\n");
            return;
        }
        int ret = ps_listinfo(plist, bufsize);
        if (ret == -1) {
            free(plist);
            bufsize *= 2;
            continue;
        } else if (ret == -2) {
            free(plist);
            printf("Test #3 (success run test) failed! Return code: %d\n", ret);
            return;
        } else {
            break;
        }
    }
    printf("Test #3 (success run test) success!\n");
}

int main() {
    test1();
    test2();
    test3();
    exit(0);
}