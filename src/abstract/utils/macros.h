#ifndef ELF_TOOLCHAIN_MACROS_H
#define ELF_TOOLCHAIN_MACROS_H

#define ROUND_DOWN(addr, page_sz) ( (addr) & ~((page_sz) - 1) )
#define ROUND_UP(addr, page_sz) ( ((addr) + (page_sz) - 1) & ~((page_sz) - 1) )

#endif //ELF_TOOLCHAIN_MACROS_H
