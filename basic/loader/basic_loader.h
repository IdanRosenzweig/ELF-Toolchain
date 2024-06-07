#ifndef LOADER_BASIC_LOADER_H
#define LOADER_BASIC_LOADER_H

#include "../utils/basic_file.h"

#include <memory>
using namespace std;

class basic_loader {
public:
    virtual void load_and_run_file(unique_ptr<basic_file> &&file) = 0;
};

#endif //LOADER_BASIC_LOADER_H
