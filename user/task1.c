#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char** argv) {
    int pid = fork();
    if (pid == 0) {
        // child
        sleep(10);
        exit(1);
    }
    if (pid < 0) {
        // fork error
        fprintf(2, "error occured during fork()\n");
        exit(5);
    }
    
    // parent
    int parent_pid = getpid();
    printf("parent_pid = %d, child_pid = %d\n", parent_pid, pid);

    // (a)
    int status, wpid;
    while ((wpid = wait(&status)) > 0);
    printf("wpid = %d, status = %d\n", wpid, status);
    exit(0);   
}