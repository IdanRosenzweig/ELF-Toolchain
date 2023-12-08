#include "include/stat.h"
#include "include/fcntl.h"
#include "include/unistd.h"
#include "unix_based_loader.h"

opened_unix_file open_file(FILE_ID id) {
    opened_unix_file file{};

    struct stat file_stat{};
    if (stat(id, &file_stat) == -1) throw "error reading file stat";

    size_t size = file_stat.st_size;
    if (size == 0) throw "file is empty";

    const int fd = open(id, O_RDONLY);
    if (fd == -1) throw "can't open file";

    char *ptr = new char[size];
//        if (ptr == nullptr) throw "can't allocate file";

    ssize_t no_read = read(fd, (void*) ptr, size);
    if (no_read == -1 || (size_t) no_read != size) throw "can't read file";
    if (close(fd) == -1) throw "can't close file";

    file.buff = ptr;

    return file;
}