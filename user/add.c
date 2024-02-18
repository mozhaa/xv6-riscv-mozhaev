#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int add_from_buf(char* buf) {
    int x = atoi(buf);
    int i = 0;
    while (buf[i] <= '9' && buf[i] >= '0')
        ++i;
    while (!(buf[i] <= '9' && buf[i] >= '0'))
        ++i;
    int y = atoi(buf + i);
    return x + y;
}

// Task (b)
void add_b() {
    char buf[32];
    gets(buf, sizeof(buf));
    printf("%d\n", add_from_buf(buf));
}

// Task (c)
void add_c() {
    char buf[32];
    int m = 0;
    while (read(0, buf + m, 1) > 0) {
        if (buf[m] == '\n') { 
            buf[m] = '\0';
            break; 
        }
        ++m;
    }
    printf("%d\n", add_from_buf(buf));
}


int main() {
    add_b();
    exit(0);   
}