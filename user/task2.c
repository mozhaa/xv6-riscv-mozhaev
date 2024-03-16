#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char** argv) {

    int fd[2];
    if (pipe(fd) < 0) {
        fprintf(2, "Error occured during pipe()\n");
        exit(6);
    }

    int pid = fork();
    if (pid == 0) {
        // child

        close(fd[1]);
        char buf[128];
        int len;
        while ((len = read(fd[0], buf, sizeof(buf))) > 0) {
            write(1, buf, len);
        }
        if (len < 0) {
            fprintf(2, "Error occured during read()\n");
            close(fd[0]);
            exit(2);
        }
        close(fd[0]);
        exit(0);
    }
    if (pid < 0) {
        // fork error
        fprintf(2, "Error occured during fork()\n");
        exit(5);
    }
    
    // parent
    close(fd[0]);
    for (int i = 1; i < argc; i++) {
        int ret1 = write(fd[1], argv[i], strlen(argv[i]));
        int ret2 = write(fd[1], "\n", 1);
        if (ret1 < 0 || ret2 < 0) {
            fprintf(2, "Error occured during write()\n");
            close(fd[1]);
            exit(3);
        }
    } 
    close(fd[1]);
    int status;
    wait(&status);
    exit(0);
}