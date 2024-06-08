#ifndef LOADER_MACROS_H
#define LOADER_MACROS_H

#define ROUND_DOWN(addr, page_sz) ( (addr) & ~((page_sz) - 1) )
#define ROUND_UP(addr, page_sz) ( ((addr) + (page_sz) - 1) & ~((page_sz) - 1) )

#endif //LOADER_MACROS_H
