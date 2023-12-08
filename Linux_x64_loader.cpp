#include <iostream>
#include "unix_based/linux_x64/linux_x64_loader.h"

int main(int argc, char *argv[], char *env[]) {

    if (argc < 2) {
        puts("give the path for the ELF file as the first argument\n");
        return 0;
    }

    linux_x64_loader loader;

    loader.setFileId(argv[1]);
    loader.setProcVar({argc - 1, &argv[1], env});
    loader.unix_based_elf_loader::setLoadFlags({false});

    try {
        loader.load_and_run();
    } catch (const char *msg) {
        std::cout << "Exception: " << msg << "\n";
    } catch (...) {
        std::cout << "unknown exception\n";
    }

    return 0;
}