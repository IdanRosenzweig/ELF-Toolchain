#ifndef ELF_TOOLCHAIN_CHECK_FILE_FORMAT_H
#define ELF_TOOLCHAIN_CHECK_FILE_FORMAT_H

#include "../abstract/utils/raw_file.h"
#include "exec_file_format.h"

executable_file_format find_executable_format(raw_file &file);


#endif //ELF_TOOLCHAIN_CHECK_FILE_FORMAT_H
