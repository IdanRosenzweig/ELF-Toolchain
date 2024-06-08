#include "../../linux_x64/elf/linux_x64_elf_loader.h"

#include <iostream>
#include <boost/program_options.hpp>
using namespace std;

int main(int argc, char *argv[], char *env[]) {
    if (argc < 2) {
        cout << "provide a path for the ELF executable file and (optionally) its command line arguments" << endl;
        return 1;
    }

    try {
        // loading
        linux_x64_elf_loader loader;

        loader.basic_unix_elf_loader::setProcVar({argc - 1, argv + 1, env});
        loader.basic_elf_loader::setLoadFlags({false});
        loader.load_and_run_file(make_unique<opened_unix_file>(argv[1]));

    } catch (const char *msg) {
        std::cout << "Exception: " << msg << "\n";
    } catch (...) {
        std::cout << "unknown exception\n";
    }
}