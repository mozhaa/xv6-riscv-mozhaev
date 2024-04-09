#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFSIZE 4096 * 4 + 1

char* fix_buffer(char* buf) {
    char* pbuf = buf;
    if (*pbuf != '[') // if start with '[', don't skip until next EOL
        // skip until next EOL
        while ((pbuf - buf < BUFSIZE) && *(pbuf++) != '\n');
    char* ret = pbuf;
    
    // find last EOL, and set byte after it to '\0'
    char* prev_endl = pbuf;
    while (1) { 
        while (*pbuf && *(pbuf++) != '\n');
        if (!*pbuf)
            break;
        prev_endl = pbuf;
    }
    *prev_endl = 0;
    return ret;
}

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

    char* pbuf = fix_buffer(buf);

    printf("%s", pbuf);
    free(buf);
    exit(0);
}