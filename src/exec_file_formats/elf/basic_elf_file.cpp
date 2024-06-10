#include "basic_elf_file.h"

bool check_elf_magic(raw_file &file) {
    // the identity fields are identical in every elf class type
    auto elf_header = reinterpret_cast<Elf64_Ehdr* >(file.offset_in_file(0));
    return elf_header->e_ident[EI_MAG0] == ELFMAG0 &&
           elf_header->e_ident[EI_MAG1] == ELFMAG1 &&
           elf_header->e_ident[EI_MAG2] == ELFMAG2 &&
           elf_header->e_ident[EI_MAG3] == ELFMAG3;
}

int check_elf_class(raw_file &file) {
    // the identity fields are identical in every elf class type
    auto elf_header = reinterpret_cast<Elf64_Ehdr *>(file.offset_in_file(0));
    switch (elf_header->e_ident[EI_CLASS]) {
        case ELFCLASS64: return 64;
        case ELFCLASS32: return 32;
        default:
            throw "invalid elf class";
    }
}