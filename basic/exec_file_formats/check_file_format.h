#ifndef LOADER_CHECK_FILE_FORMAT_H
#define LOADER_CHECK_FILE_FORMAT_H

#include "../utils/basic_file.h"
#include "exec_file_format.h"

#include "../elf/basic_elf_file.h"

executable_file_format find_executable_format(const unique_ptr<basic_file> &file);


#endif //LOADER_CHECK_FILE_FORMAT_H
