#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/riscv.h"
#include "user/user.h"

int assert_accessed(const char* buf, int pages, const char* expected, int assert_num) {
    for (int i = 0; i < pages; ++i) {
        if (buf[i] != expected[i]) {
            fprintf(2, "ASSERT #%d FAILURE: pgaccess returned %d on index #%d, expected: %d\n", assert_num, buf[i], i, expected[i]);
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


int test_array_stack() {

#define PAGES 4

    int ret = 0;
    char accessed[PAGES];
    char array[PGSIZE * PAGES];

    if (!safepgaccess(array, PAGES, accessed)) {
        ret = 10;
        goto test_array_stack_end;
    }
    if (!assert_accessed(accessed, PAGES, "\0\0\0\0", 1)) {
        ret = 1;
    }

    for (int i = 0; i < PGSIZE * PAGES; ++i)
        array[i] = 0;
    
    if (!safepgaccess(array, PAGES, accessed)) {
        ret = 10;
        goto test_array_stack_end;
    }
    if (!assert_accessed(accessed, PAGES, "\1\1\1\1", 2)) {
        ret = 1;
    }

    array[PGSIZE * 0] += 1;
    array[PGSIZE * 3] += 1;

    if (!safepgaccess(array, PAGES, accessed)) {
        ret = 10;
        goto test_array_stack_end;
    }
    if (!assert_accessed(accessed, PAGES, "\1\0\0\1", 3)) {
        ret = 1;
    }

    array[PGSIZE * 1] += 1;
    array[PGSIZE * 2] += 1;
    array[PGSIZE * 3] += 1;

    if (!safepgaccess(array, PAGES, accessed)) {
        ret = 10;
        goto test_array_stack_end;
    }
    if (!assert_accessed(accessed, PAGES, "\0\1\1\1", 4)) {
        ret = 1;
    }

test_array_stack_end:
    return ret;
    return 0;
}

int test_array_heap() {

#define PAGES 4

    int ret = 0;
    char accessed[PAGES];
    char* array = malloc(PGSIZE * PAGES * sizeof(char));

    if (safepgaccess(array, PAGES, accessed)) {
        ret = 10;
        goto test_array_heap_end;
    }
    if (!assert_accessed(accessed, PAGES, "\0\0\0\0", 1)) {
        ret = 1;
    }

    for (int i = 0; i < PGSIZE * PAGES; ++i)
        array[i] = 0;
    
    if (safepgaccess(array, PAGES, accessed)) {
        ret = 10;
        goto test_array_heap_end;
    }
    if (!assert_accessed(accessed, PAGES, "\1\1\1\1", 2)) {
        ret = 1;
    }

    array[PGSIZE * 0] += 1;
    array[PGSIZE * 3] += 1;

    if (safepgaccess(array, PAGES, accessed)) {
        ret = 10;
        goto test_array_heap_end;
    }
    if (!assert_accessed(accessed, PAGES, "\1\0\0\1", 3)) {
        ret = 1;
    }

    array[PGSIZE * 1] += 1;
    array[PGSIZE * 2] += 1;
    array[PGSIZE * 3] += 1;

    if (safepgaccess(array, PAGES, accessed)) {
        ret = 10;
        goto test_array_heap_end;
    }
    if (!assert_accessed(accessed, PAGES, "\0\1\1\1", 4)) {
        ret = 1;
    }

test_array_heap_end:
    free(array);
    return ret;
}

int main() {
    int ret = 0;
    // ret |= test_array_stack();
    ret |= test_array_heap();

    if (!ret) {
        printf("All tests successed.\n");
    }

    exit(ret);
}