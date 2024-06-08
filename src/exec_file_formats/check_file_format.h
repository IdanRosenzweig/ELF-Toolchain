#ifndef LOADER_CHECK_FILE_FORMAT_H
#define LOADER_CHECK_FILE_FORMAT_H

#include "../abstract/utils/raw_file.h"
#include "exec_file_format.h"

executable_file_format find_executable_format(raw_file &file);


#endif //LOADER_CHECK_FILE_FORMAT_H
