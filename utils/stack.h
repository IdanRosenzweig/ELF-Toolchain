#ifndef LOADER_STACK_H
#define LOADER_STACK_H

struct stack {
    void* ptr;
    unsigned long long size;
    int prot_flags;
};

#endif //LOADER_STACK_H
