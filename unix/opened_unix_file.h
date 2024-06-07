#ifndef LOADER_OPENED_UNIX_FILE_H
#define LOADER_OPENED_UNIX_FILE_H

#include "../basic/utils/basic_file.h"
#include "../basic/utils/udata.h"

#include <string>
using namespace std;

class opened_unix_file : public basic_file {
public:
    string path;
    udata file_data;

    opened_unix_file() {}
    explicit opened_unix_file(const string& path);

    uint8_t *offset_in_file(size_t offset) override;

    size_t file_size() override;

    string file_path() override;

};


#endif //LOADER_OPENED_UNIX_FILE_H