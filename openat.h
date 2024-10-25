#ifndef __OPENAT_H__
#define __OPENAT_H__

struct openat_evt {
    pid_t pid;
    pid_t tgid;
    char comm[32];
    char filename[256];
};

#endif // __OPENAT_H__