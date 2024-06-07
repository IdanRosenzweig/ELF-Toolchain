#ifndef LOADER_BASIC_FILE_H
#define LOADER_BASIC_FILE_H

#include <cstdlib>
#include <stdint.h>
#include <string>
using namespace std;

struct basic_file {
    // returns readable, accessible pointer to the data at offset of the file
    virtual uint8_t * offset_in_file(size_t offset) = 0;

    virtual size_t file_size() = 0;

    virtual string file_path() = 0;

    virtual ~basic_file() {}
};

#endif //LOADER_BASIC_FILE_H
