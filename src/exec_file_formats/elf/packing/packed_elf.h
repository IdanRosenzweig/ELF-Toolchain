#ifndef ELF_TOOLCHAIN_PACKED_ELF_H
#define ELF_TOOLCHAIN_PACKED_ELF_H

#include "../../../abstract/packing/basic_packed_file.h"
#include "../../../abstract/utils/raw_file.h"

#include "../../../abstract/loading/basic_loader.h"

#include <cstring>

using namespace std;

class packed_elf : public basic_packed_file {
public:
    basic_loader *loader;

    packed_elf() = delete;

    packed_elf(basic_loader *loader) : loader(loader) {}

    void handle_data(const udata &data, const string &original_path) override {
        // wrapping the file's data as a raw file
        raw_file file;
        file.content = data;
        file.path = original_path;

        // load the file
        loader->load_and_run_file(std::move(file));
    }

};


#endif //ELF_TOOLCHAIN_PACKED_ELF_H
