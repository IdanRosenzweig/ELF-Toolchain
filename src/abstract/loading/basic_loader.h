#ifndef ELF_TOOLCHAIN_BASIC_LOADER_H
#define ELF_TOOLCHAIN_BASIC_LOADER_H

#include "../utils/raw_file.h"

#include <memory>

using namespace std;

class basic_loader {
public:
    virtual void load_and_run_file(raw_file &&file) = 0;

    virtual ~basic_loader() {}
};

#endif //ELF_TOOLCHAIN_BASIC_LOADER_H
