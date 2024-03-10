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
        int ret = read(fd[0], buf, sizeof(buf));
        if (ret > 0) { // read success
            printf("%s", buf);
            close(fd[0]);
            exit(0);
        } else if (ret == 0) { // eof
            close(fd[0]);
            exit(1);
        } else { // read error
            fprintf(2, "Error occured during read()\n");
            close(fd[0]);
            exit(2);
        }
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
    
    int ret = write(fd[1], buf, sizeof(buf));
    if (ret > 0) { // write success
        close(fd[1]);
        int status;
        wait(&status);
        exit(0);
    } else { // write error
        fprintf(2, "Error occured during write()\n");
        close(fd[1]);
        exit(3);
    }
}