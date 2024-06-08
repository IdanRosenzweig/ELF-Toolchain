#ifndef LOADER_STACK_H
#define LOADER_STACK_H

#include <cstdlib>

// stack can grow upwards or downwards
struct stack {
    void *ptr;
    size_t size;
};

#endif //LOADER_STACK_H
