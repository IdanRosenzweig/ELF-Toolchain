#ifndef LOADER_OPENED_UNIX_FILE_H
#define LOADER_OPENED_UNIX_FILE_H

#include "../exec_file_formats/exec_file_format.h"

struct opened_unix_file {
    void *buff;

    void *get_offset(unsigned long long) const;
};

executable_file_format check_format(const opened_unix_file &);

#endif //LOADER_OPENED_UNIX_FILE_H
