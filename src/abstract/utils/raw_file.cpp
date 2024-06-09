#include "raw_file.h"

#include <fstream>

uint8_t *raw_file::offset_in_file(size_t offset) {
    return content.data() + offset;
}

size_t raw_file::file_size() const {
    return content.size();
}

string raw_file::file_path() const {
    return path;
}

raw_file open_raw_file(const string &path) {
    raw_file file;

    ifstream f(path, ios::binary);
    file.content = udata((istreambuf_iterator<char>(f)),
                         istreambuf_iterator<char>());
    f.close();

    file.path = path;

    return file;
}

void store_to_file_system(const string &path, const udata &content) {
    ofstream f(path, ios::binary | ios::trunc);
    f.write((const char *) content.data(), content.size());
    f.close();
}