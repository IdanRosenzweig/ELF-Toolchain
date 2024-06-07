#include "file_helper_funcs.h"

#include "error_codes.h"
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>

void write_data_to_file(const udata& data, const string& file_path) {
    int fd = open(file_path.c_str(), O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd == OPEN_ERROR) throw "can't open file";

    ssize_t no_bytes_written = write(fd, data.data(), data.size());
    if (no_bytes_written == WRITE_ERROR || (size_t) no_bytes_written != data.size()) throw "can't write whole file";

    if (close(fd) == CLOSE_ERROR) throw "can't close file";
}

udata get_data_from_file(const string& file_path) {
    struct stat file_stat{};
    if (stat(file_path.c_str(), &file_stat) == STAT_ERROR) throw "error reading file stat";

    size_t file_sz = file_stat.st_size;
    if (file_sz == 0) throw "file is empty";

    int fd = open(file_path.c_str(), O_RDONLY);
    if (fd == OPEN_ERROR) throw "can't open file";

    udata file_content(file_sz, 0);

    ssize_t no_bytes_read = read(fd, file_content.data(), file_sz);
    if (no_bytes_read == READ_ERROR || (size_t) no_bytes_read != file_sz) throw "can't read file";

    if (close(fd) == CLOSE_ERROR) throw "can't close file";

    return file_content;
}
