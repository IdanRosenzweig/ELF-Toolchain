#include "check_file_format.h"

executable_file_format find_executable_format(const unique_ptr<basic_file> &file) {
//    basic_elf_file<64> as_elf_file(raw_file(raw_file));
//    Elf64_Ehdr *elf_header = (Elf64_Ehdr *) (raw_file.get_offset(0));
//    if (elf_header->e_ident[EI_MAG0] == ELFMAG0 &&
//        elf_header->e_ident[EI_MAG1] == ELFMAG1 &&
//        elf_header->e_ident[EI_MAG2] == ELFMAG2 &&
//        elf_header->e_ident[EI_MAG3] == ELFMAG3) // valid elf magic bytes
//        return executable_file_format::ELF; // type of elf

    return executable_file_format::ELF;

    // pe

    throw "no standard executable file format found";
}