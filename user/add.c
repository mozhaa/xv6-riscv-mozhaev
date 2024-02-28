#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUF_SIZE 32

int add_from_buf(char* buf) {
    int x = atoi(buf);
    int i = 0;
    while (buf[i] <= '9' && buf[i] >= '0') {
        ++i;
        if (i == BUF_SIZE) {
            fprintf(2, "Buffer overflow!\n");
            exit(1); // Buffer overflow
        }
    }
    if (i == 0) {
        fprintf(2, "Invalid format!\n");
        exit(3); // Not a number in the beginning
    }
    int prev_i = i;
    while (!(buf[i] <= '9' && buf[i] >= '0')) {
        ++i;
        if (i == BUF_SIZE) {
            fprintf(2, "Buffer overflow!\n");
            exit(1); // Buffer overflow
        }
    }
    if (i == prev_i) {
        fprintf(2, "Invalid format!\n");
        exit(3); // No second number
    }
    int y = atoi(buf + i);
    return x + y;
}

// Task (b)
void add_b() {
    char buf[BUF_SIZE];
    char* ret = gets(buf, sizeof(buf));
    if (ret - buf > BUF_SIZE) {
        fprintf(2, "Buffer overflow!\n");
        exit(1); // Buffer overflow
    }
    if (ret == 0) {
        fprintf(2, "Input error!\n");
        exit(2); // Input error
    }
    printf("%d\n", add_from_buf(buf));
}

// Task (c)
void add_c() {
    char buf[BUF_SIZE];
    int m = 0, res;
    while (1) {
        res = read(0, buf + m, 1);
        if (res == 0) {
            break;
        }
        if (res < 0) {
            fprintf(2, "Read error!\n");
            exit(4); // Read error
        }
        if (buf[m] == '\n') { 
            buf[m] = '\0';
            break; 
        } else if (m == BUF_SIZE - 1) {
            fprintf(2, "Buffer overflow!\n");
            exit(1); // Buffer overflow
        }
        ++m;
    }
    printf("%d\n", add_from_buf(buf));
}

// Task 2
void add_sys_call_test() {
    char buf[BUF_SIZE];
    char* ret = gets(buf, sizeof(buf));
    if (ret - buf > BUF_SIZE) {
        fprintf(2, "Buffer overflow!\n");
        exit(1); // Buffer overflow
    }
    if (ret == 0) {
        fprintf(2, "Input error!\n");
        exit(2); // Input error
    }
    int x = atoi(buf);
    int i = 0;
    while (buf[i] <= '9' && buf[i] >= '0') {
        ++i;
        if (i == BUF_SIZE) {
            fprintf(2, "Buffer overflow!\n");
            exit(1); // Buffer overflow
        }
    }
    if (i == 0) {
        fprintf(2, "Invalid format!\n");
        exit(3); // Not a number in the beginning
    }
    int prev_i = i;
    while (!(buf[i] <= '9' && buf[i] >= '0')) {
        ++i;
        if (i == BUF_SIZE) {
            fprintf(2, "Buffer overflow!\n");
            exit(1); // Buffer overflow
        }
    }
    if (i == prev_i) {
        fprintf(2, "Invalid format!\n");
        exit(3); // No second number
    }
    int y = atoi(buf + i);
    int s;
    add(&s, x, y);
    printf("%d\n", s);
}

int main(int argc, char** argv) {

    // Usage: `add [0/1/2]`, where
    // 0 - using gets (default)
    // 1 - using read
    // 2 - using custom syscall for summing numbers

    if (argc > 1) {
        if (argv[1][0] == '0') {
            add_b();
        } else if (argv[1][0] == '1') {
            add_c();
        } else if (argv[1][0] == '2') {
            add_sys_call_test();
        }
    } else {
        add_b();
    }
    exit(0);   
}