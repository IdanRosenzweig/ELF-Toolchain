#ifndef LOADER_BASIC_PACKER_H
#define LOADER_BASIC_PACKER_H

#include "../utils/basic_file.h"

#include <memory>
using namespace std;

class basic_packer {
public:
    virtual unique_ptr<basic_file> pack_file(unique_ptr<basic_file> &&file) = 0;
};

#endif //LOADER_BASIC_PACKER_H
