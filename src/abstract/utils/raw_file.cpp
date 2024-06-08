#include "raw_file.h"

uint8_t *raw_file::offset_in_file(size_t offset) {
    return data.data() + offset;
}

size_t raw_file::file_size() {
    return data.size();
}

string raw_file::file_path() {
    return path;
}