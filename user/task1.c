#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char** argv) {

    // Usage: task1 [b]
    // default - variant (a)
    // [b] - variant (b) is used (kill child process)

    int variant_b = 0;
    if (argc > 1 && argv[1][0] == 'b') variant_b = 1;

    int pid = fork();
    if (pid == 0) {
        // child
        sleep(50);
        exit(1);
    }
    if (pid < 0) {
        // fork error
        fprintf(2, "Error occured during fork()\n");
        exit(5);
    }
    
    // parent
    int parent_pid = getpid();
    printf("Parent's PID:\t%d\nChild's PID:\t%d\n...\n", parent_pid, pid);

    if (variant_b) {
        kill(pid);
    }

    int status;
    int wpid = wait(&status);
    printf("Finished PID:\t%d\nExit status:\t%d\n", wpid, status);
    exit(0);
}