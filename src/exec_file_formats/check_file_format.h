#ifndef LOADER_CHECK_FILE_FORMAT_H
#define LOADER_CHECK_FILE_FORMAT_H

#include "../abstract/utils/basic_file.h"
#include "exec_file_format.h"

#include <memory>
using namespace std;

executable_file_format find_executable_format(const unique_ptr<basic_file> &file);


#endif //LOADER_CHECK_FILE_FORMAT_H
