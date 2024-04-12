#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define HELPMSG \
"Usage: `prconfig <-e(nable)/-d(isable)> [-t timeout] <modes>`\n" \
"   modes: [s][d][p][e]\n" \
"       s - system calls\n" \
"       d - dev interrupts\n" \
"       p - proc switches\n" \
"       e - exec calls\n" \
"Examples:\n" \
"   `prconfig -e s` -- enable s mode\n" \
"   `prconfig -e -t 1000 d` -- enable d mode for 1000 ticks\n" \
"   `prconfig -d sdpe` -- disable all modes\n"

const char* modeslist = "sdpe";

int main(int argc, char** argv) {
    if (argc == 1) {
        printf(HELPMSG);
        exit(-1);
    }

    int enable = 1, modes = 0, timeout = 0;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (argv[i][2] != 0) {
                fprintf(2, "Unknown flag: \"%s\".\n", argv[i]);
                exit(-2);
            }
            if (argv[i][1] == 'e') {
                enable = 1;
                continue;
            }
            if (argv[i][1] == 'd') {
                enable = 0;
                continue;
            }
            if (argv[i][1] == 't') {
                timeout = atoi(argv[++i]);
                continue;
            }
        }

        for (char* c = argv[i]; *c; ++c) {
            int found_mode = 0, mask = 1;
            for (const char* m = modeslist; *m; ++m, mask <<= 1) {
                if (*c == *m) {
                    if (modes & mask) {
                        fprintf(2, "Every mode flag can be set only once.\n");
                        exit(-3);
                    } else {
                        modes ^= mask;
                        found_mode = 1;
                        break;
                    }
                }
            }
            if (!found_mode) {
                fprintf(2, "Unknown mode: \"%c\"\n", *c);
                exit(-4);
            }
        } 
        
    }

    if (!enable) {
        timeout = -1;
    }

    printf("debug: %d, %d\n", modes, timeout);
    prset(modes, timeout);

    exit(0);
}