#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define PAGES 4

int assert_accessed(const char* buf, int pages, const char* expected, int assert_num) {
    for (int i = 0; i < pages; ++i) {
        if (buf[i] != expected[i]) {
            fprintf(2, "ASSERT #%d FAILURE:\n", assert_num);
            fprintf(2, "\tACTUAL:\t\t");
            for (int j = 0; j < pages; ++j)
                fprintf(2, "%d", buf[j]);
            fprintf(2, "\n\tEXPECTED:\t");
            for (int j = 0; j < pages; ++j)
                fprintf(2, "%d", expected[j]);
            fprintf(2, "\n");
            return 0;
        }
    }
    return 1;
}

int safepgaccess(void* paddr, int pages, char* buf) {
    if (pgaccess(paddr, pages, buf)) {
        fprintf(2, "ERROR: sys_pgaccess() failure\n");
        return 1;
    }
    return 0;
}

int common_test(char* array) {

#define __assert(assert_num, expected) \
    do { \
        if (safepgaccess(array, PAGES, accessed)) \
            return 2; \
        if (!assert_accessed(accessed, PAGES, expected, assert_num)) \
            ret = 1; \
    } while(0)

    int ret = 0;
    char accessed[PAGES + 1];

    // unitialized array
    __assert(1, "\0\0\0\0");

    // fill with zeros
    for (int i = 0; i < PGSIZE * PAGES; ++i)
        array[i] = 0;
    __assert(2, "\1\1\1\1");

    // modify values in pages #0 and #3
    array[PGSIZE * 0] += 1;
    array[PGSIZE * 3] += 1;
    __assert(3, "\1\0\0\1");

    // modify values in pages #1, #2 and #3
    array[PGSIZE * 1] += 1;
    array[PGSIZE * 2] += 1;
    array[PGSIZE * 3] += 1;
    __assert(4, "\0\1\1\1");
    
    // modify values in page #0
    array[0] += 1;
    __assert(5, "\1\0\0\0");

    // do nothing
    __assert(6, "\0\0\0\0");

    // modify values in page #0
    array[0] += 1;
    __assert(7, "\1\0\0\0");

    // modify values in pages #2 and #3
    array[PGSIZE * 1] += 1;
    array[PGSIZE * 2] += 1;
    __assert(8, "\0\1\1\0");

    // do nothing
    __assert(9, "\0\0\0\0");

    // do nothing again
    __assert(10, "\0\0\0\0");

    // fill page #3
    for (int i = PGSIZE * 3; i < PGSIZE * 4; ++i)
        array[i] = 66;
    __assert(11, "\0\0\0\1");

    return ret;

#undef __assert

}

char xarray[PGSIZE * PAGES];

/* Test pgaccess() for array allocated on stack */
int test_array_stack() {
    return common_test(xarray);
}

/* Test pgaccess() for array allocated on heap */
int test_array_heap() {
    char* xarray = malloc(PGSIZE * PAGES * sizeof(char));
    if (xarray == 0) {
        fprintf(2, "malloc() failure\n");
        return 4;
    }
    int ret = common_test(xarray);
    free(xarray);
    return ret;
}

int main() {
    int ret = 0;
    ret |= test_array_stack();
    ret |= test_array_heap();

    if (!ret) {
        printf("All tests successed.\n");
    }

    exit(ret);
}