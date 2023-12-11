#ifndef LOADER_MACROS_H
#define LOADER_MACROS_H

#define ROUND_DOWN(x, s) ( (x) & ~((s) - 1) )
#define ROUND_UP(x, s) ( ((x) + (s) - 1) & ~((s) - 1) )

#endif //LOADER_MACROS_H
