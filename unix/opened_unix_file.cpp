#include "opened_unix_file.h"

#include "error_codes.h"
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <unistd.h>
#include <iostream>

opened_unix_file::opened_unix_file(const string &p) : path(p) {
    struct stat file_stat{};
    if (stat(path.c_str(), &file_stat) == STAT_ERROR) throw "error reading file stat";

    size_t file_sz = file_stat.st_size;
    if (file_sz == 0) throw "file is empty";

    int fd = open(path.c_str(), O_RDONLY);
    if (fd == OPEN_ERROR) throw "can't open file";

    file_data = udata(file_sz, 0);

    ssize_t no_bytes_read = read(fd, file_data.data(), file_sz);
    if (no_bytes_read == READ_ERROR || (size_t) no_bytes_read != file_sz) throw "can't read whole file";

    if (close(fd) == CLOSE_ERROR) throw "can't close file";
}

uint8_t *opened_unix_file::offset_in_file(size_t offset) {
    return file_data.data() + offset;
}

size_t opened_unix_file::file_size() {
    return file_data.size();
}

string opened_unix_file::file_path() {
    return path;
}
