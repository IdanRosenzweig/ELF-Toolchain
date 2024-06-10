#ifndef ELF_TOOLCHAIN_STACK_H
#define ELF_TOOLCHAIN_STACK_H

#include <cstdlib>

// stack can grow upwards or downwards
struct stack {
    void *ptr;
    size_t size;
};

#endif //ELF_TOOLCHAIN_STACK_H
