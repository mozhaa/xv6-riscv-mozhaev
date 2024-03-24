#ifndef MUTEX_H
#define MUTEX_H

struct mutex {
    int proc_count;
    struct sleeplock sl;
};

#endif // MUTEX_H