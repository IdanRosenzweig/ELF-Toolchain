#include "../../../abstract/utils/raw_file.h"

#include "../../../exec_file_formats/elf/stripping/elf_stripper.h"

#include <iostream>

using namespace std;

int main(int argc, char *argv[], char *env[]) {
    if (argc < 2) {
        cout << "provide a path for the ELF executable" << endl;
        return 1;
    }

    try {
        raw_file file = open_raw_file(argv[1]);

        raw_file generated_file;
        switch (check_elf_class(file)) {
            case 64: {
                // open 64 bit elf
                auto elf = parse_elf_from_raw_file<64>(open_raw_file(argv[1]));

                // stripp the file
                strip_elf_file<64>(elf);

                generated_file = generate_file_from_custom_elf<64>(std::move(elf));
                break;
            }
            case 32: {
                // open 32 bit elf
                auto elf = parse_elf_from_raw_file<32>(open_raw_file(argv[1]));

                // strip the file
                strip_elf_file<32>(elf);

                generated_file = generate_file_from_custom_elf<32>(std::move(elf));
                break;
            }
        }

        store_to_file_system("stripped_file", generated_file.content);

    } catch (const char *msg) {
        std::cout << "Exception: " << msg << "\n";
    } catch (...) {
        std::cout << "unknown exception\n";
    }
}