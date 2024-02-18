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
            exit(1); // Buffer overflow
        }
    }
    if (i == 0) {
        exit(3); // Not a number in the beginning
    }
    int prev_i = i;
    while (!(buf[i] <= '9' && buf[i] >= '0')) {
        ++i;
        if (i == BUF_SIZE) {
            exit(1); // Buffer overflow
        }
    }
    if (i == prev_i) {
        exit(4); // No second number
    }
    int y = atoi(buf + i);
    return x + y;
}

// Task (b)
void add_b() {
    char buf[BUF_SIZE];
    char* ret = gets(buf, sizeof(buf));
    if (ret - buf > BUF_SIZE) {
        exit(1); // Buffer overflow
    }
    if (ret == 0) {
        exit(2); // Input error
    }
    printf("%d\n", add_from_buf(buf));
}

// Task (c)
void add_c() {
    char buf[BUF_SIZE];
    int m = 0;
    while (read(0, buf + m, 1) > 0) {
        if (buf[m] == '\n') { 
            buf[m] = '\0';
            break; 
        } else if (m == BUF_SIZE - 1) {
            exit(1); // Buffer overflow
        }
        ++m;
    }
    printf("%d\n", add_from_buf(buf));
}


int main() {
    add_b();
    exit(0);   
}