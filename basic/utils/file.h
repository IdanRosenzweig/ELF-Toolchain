#ifndef LOADER_FILE_H
#define LOADER_FILE_H

#include <cstdlib>
#include <stdint.h>

struct file {
    // returns readable, accessible pointer to the data at offset of the raw_file
    virtual uint8_t * offset_in_file(size_t offset) = 0;

    virtual ~file() {}
};

#endif //LOADER_FILE_H
