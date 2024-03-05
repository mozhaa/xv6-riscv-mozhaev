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

        char buf[100];
        read(fd[0], buf, sizeof(buf));
        printf("%s", buf);
        close(fd[0]); // close output side of pipe
        exit(0);
    }
    if (pid < 0) {
        // fork error
        fprintf(2, "Error occured during fork()\n");
        exit(5);
    }
    
    // parent
    char buf[100];
    char* pbuf = buf;
    for (int i = 1; i < argc; ++i) {
        strcpy(pbuf, argv[i]);
        pbuf += strlen(argv[i]);
        *(pbuf++) = ' ';
    }
    *(pbuf++) = '\n';
    *(pbuf++) = '\0';
    
    write(fd[1], buf, sizeof(buf));
    close(fd[1]); // close output side of pipe

    int status;
    wait(&status);
    exit(0);
}