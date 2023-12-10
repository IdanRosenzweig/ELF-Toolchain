#include "include/stat.h"
#include "include/fcntl.h"
#include "include/unistd.h"
#include "include/error_codes.h"
#include "unix_based_loader.h"

opened_unix_file open_file(FILE_ID id) {
    opened_unix_file file{};

    struct stat file_stat{};
    if (stat(id, &file_stat) == STAT_ERROR) throw "error reading file stat";

    size_t size = file_stat.st_size;
    if (size == 0) throw "file is empty";

    int fd = open(id, O_RDONLY);
    if (fd == OPEN_ERROR) throw "can't open file";

    char *ptr = new char[size];
//        if (ptr == nullptr) throw "can't allocate file";

    ssize_t no_bytes_read = read(fd, (void*) ptr, size);
    if (no_bytes_read == READ_ERROR || (size_t) no_bytes_read != size) throw "can't read file";

    if (close(fd) == CLOSE_ERROR) throw "can't close file";

    file.buff = ptr;

    return file;
}