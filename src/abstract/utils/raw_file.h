#ifndef LOADER_RAW_FILE_H
#define LOADER_RAW_FILE_H

#include "udata.h"

#include <string>

using namespace std;

class raw_file {
public:
    udata content;
    string path;

    uint8_t *offset_in_file(size_t offset);

    size_t file_size() const;

    string file_path() const;

    raw_file() {}

    raw_file(raw_file &&other) : content(std::move(other.content)), path(std::move(other.path)) {
    }

    raw_file &operator=(raw_file &&other)

    noexcept {
        this->content = std::move(other.content);
        this->path = std::move(other.path);

        return *this;
    }
};

raw_file open_raw_file(const string &path);

void store_to_file_system(const string &path, const udata &content);

#endif //LOADER_RAW_FILE_H
