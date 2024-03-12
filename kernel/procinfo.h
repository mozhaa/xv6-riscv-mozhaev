#ifndef procinfo_h
#define procinfo_h

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct procinfo {
    char name[16];
    enum procstate state;
    int parent_pid;
};

#endif // procinfo_h