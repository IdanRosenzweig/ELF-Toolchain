#ifndef LOADER_FILE_HELPER_FUNCS_H
#define LOADER_FILE_HELPER_FUNCS_H

#include "../basic/utils/udata.h"

#include <stdint.h>
#include <string>
using namespace std;

void write_data_to_file(const udata& data, const string& file_path);

udata get_data_from_file(const string& file_path);

#endif //LOADER_FILE_HELPER_FUNCS_H
