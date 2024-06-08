#ifndef LOADER_RAW_FILE_H
#define LOADER_RAW_FILE_H

#include "basic_file.h"
#include "udata.h"

class raw_file : public basic_file {
public:
    udata data;
    string path;

    uint8_t *offset_in_file(size_t offset) override;

    size_t file_size() override;

    string file_path() override;
};


#endif //LOADER_RAW_FILE_H
