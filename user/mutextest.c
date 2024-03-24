#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ERROR_MESSAGE(fun) fprintf(2, "Error occured during %s().\n", fun); 

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(2, "Usage: `mutextest <str> [-m]`\n\tUse flag '-m' for version, where printf's are secured with mutex.\n");
        exit(-1);
    }
    int use_mutex = 0;
    if (argc > 2) {
        if (!strcmp(argv[2], "-m")) {
            use_mutex = 1;
        }
    }
    int mutex = -1;
    if (use_mutex) {
        mutex = mutex_create();
        if (mutex < 0) {
            ERROR_MESSAGE("mutex");
            exit(-8);
        }
    }
    int fd_down[2];
    int fd_up[2];
    if (pipe(fd_down) < 0 || pipe(fd_up) < 0) {
        ERROR_MESSAGE("pipe");
        exit(-2);
    }
    int pid = fork();
    if (pid < 0) {
        ERROR_MESSAGE("fork");
        exit(-3);
    } else if (pid > 0) {
        // parent
        close(fd_up[1]);
        close(fd_down[0]);
        if (write(fd_down[1], argv[1], strlen(argv[1])) < 0) {
            ERROR_MESSAGE("write");
            exit(-4);
        }
        close(fd_down[1]);

        char c;
        int ret;
        while ((ret = read(fd_up[0], &c, 1)) > 0) {
            if (use_mutex) {
                int mret = mutex_lock(mutex);
                if (mret < 0) {
                    ERROR_MESSAGE("mutex_lock"); 
                    exit(mret); 
                }
            }
            printf("%d: received '%c'\n", getpid(), c);
            if (use_mutex) {
                int mret = mutex_unlock(mutex);
                if (mret < 0) {
                    ERROR_MESSAGE("mutex_unlock"); 
                    exit(mret); 
                }
            }
        }
        if (ret < 0) {
            ERROR_MESSAGE("read");
            exit(-7);
        }
        close(fd_up[0]);
        if (use_mutex) {
            int mret = mutex_destroy(mutex);
            if (mret < 0) {
                ERROR_MESSAGE("mutex_destroy");
                exit(mret);
            }
        }
        exit(0);

    } else {
        // child
        close(fd_up[0]);
        close(fd_down[1]);
        char c;
        int ret;
        while ((ret = read(fd_down[0], &c, 1)) > 0) {
            if (use_mutex) {
                int mret = mutex_lock(mutex);
                if (mret < 0) {
                    ERROR_MESSAGE("mutex_lock"); 
                    exit(mret); 
                }
            }
            printf("%d: received '%c'\n", getpid(), c);
            if (use_mutex) {
                int mret = mutex_unlock(mutex);
                if (mret < 0) {
                    ERROR_MESSAGE("mutex_unlock"); 
                    exit(mret); 
                }
            }
            if (write(fd_up[1], &c, 1) < 0) {
                ERROR_MESSAGE("write");
                exit(-5);
            }
        }
        if (ret < 0) {
            ERROR_MESSAGE("read");
            exit(-6);
        }
        close(fd_up[1]);
        close(fd_down[0]);
        if (use_mutex) {
            int mret = mutex_destroy(mutex);
            if (mret < 0) {
                ERROR_MESSAGE("mutex_destroy");
                exit(mret);
            }
        }
        exit(0);
    }
}