#include "../../../abstract/utils/raw_file.h"

#include "../../../exec_file_formats/elf/strip/elf_stripper.h"

#include <iostream>

using namespace std;

int main(int argc, char *argv[], char *env[]) {
    if (argc < 2) {
        cout << "provide a path for the ELF executable file and (optionally) its command line arguments" << endl;
        return 1;
    }

    try {
        auto elf = parse_elf_from_raw_file<64>(open_raw_file(argv[1]));

        // stripping the file
        strip_elf_file<64>(elf);

        auto file = generate_file_from_custom_elf<64>(std::move(elf));
        store_to_file_system("stripped_file", file.content);


    } catch (const char *msg) {
        std::cout << "Exception: " << msg << "\n";
    } catch (...) {
        std::cout << "unknown exception\n";
    }
}