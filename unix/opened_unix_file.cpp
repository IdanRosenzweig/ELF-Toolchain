#include "opened_unix_file.h"

#include "error_codes.h"
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <unistd.h>
#include <iostream>

opened_unix_file::opened_unix_file(const string &p) : path(p) {
    struct stat file_stat{};
    if (stat(path.c_str(), &file_stat) == STAT_ERROR) throw "error reading raw_file stat";

    size_t size = file_stat.st_size;
    if (size == 0) throw "raw_file is empty";

    int fd = open(path.c_str(), O_RDONLY);
    if (fd == OPEN_ERROR) throw "can't open raw_file";

    file_data = (uint8_t *) malloc(size);
    if (file_data == nullptr) throw "can't allocate buffer for raw_file's data";

    ssize_t no_bytes_read = read(fd, file_data, size);
    if (no_bytes_read == READ_ERROR || (size_t) no_bytes_read != size) throw "can't read raw_file";

    if (close(fd) == CLOSE_ERROR) throw "can't close raw_file";
}

opened_unix_file::~opened_unix_file() {
    if (file_data != nullptr) free((void *) file_data);
}

uint8_t *opened_unix_file::offset_in_file(size_t offset) {
    return file_data + offset;
}
