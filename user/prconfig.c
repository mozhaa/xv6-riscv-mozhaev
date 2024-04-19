#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define HELPMSG \
"Usage: `prconfig [-e/-d] [-t timeout] [-l] [modes]`\n" \
"   modes: [s][d][p][e]\n" \
"       s - system calls\n" \
"       d - dev interrupts\n" \
"       p - proc switches\n" \
"       e - exec calls\n" \
"       (default: all modes)\n" \
"Flags:\n" \
"   -e/-d - enable/disable modes (default: enabled)\n" \
"   -t timeout - set timeout in ticks (default: no timeout)\n" \
"   -l - enable 'long' mode (print registers dumps in 'proc switches' mode) (default: disabled)\n" \
"Examples:\n" \
"   `prconfig` -- print this help message\n" \
"   `prconfig -e s` -- enable s mode\n" \
"   `prconfig -e -t 1000 d` -- enable d mode for 1000 ticks\n" \
"   `prconfig -e -t 20 -l p` -- enable p mode for 20 ticks in 'long' mode\n" \
"   `prconfig -d sdpe` -- disable all modes\n"

const char* modeslist = "sdpe";

int main(int argc, char** argv) {
    if (argc == 1) {
        printf(HELPMSG);
        exit(-1);
    }

    int enable = 1, modes = 0, timeout = 0, option_dump = 0;
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
            if (argv[i][1] == 'l') {
                option_dump = 1;
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

    if (!modes) {
        modes = 0b1111;
    }

    prset(modes, timeout, option_dump);

    exit(0);
}