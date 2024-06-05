#ifndef LOADER_OPENED_UNIX_FILE_H
#define LOADER_OPENED_UNIX_FILE_H

#include "../basic/utils/file.h"

#include <string>
using namespace std;

class opened_unix_file : public file {
public:
    string path;
    uint8_t * file_data = nullptr;

    explicit opened_unix_file(const string& path);

    ~opened_unix_file();

    uint8_t *offset_in_file(size_t offset) override;
};


#endif //LOADER_OPENED_UNIX_FILE_H
