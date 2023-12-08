#include "elf.h"

#include "opened_unix_file.h"

void *opened_unix_file::get_offset(unsigned long long offset) const {
    return (void *) ((unsigned long long) buff + offset);
}

executable_file_format check_format(const opened_unix_file &file) {
    Elf64_Ehdr *elf_header = (Elf64_Ehdr *) (file.get_offset(0));
    if (elf_header->e_ident[EI_MAG0] == ELFMAG0 &&
        elf_header->e_ident[EI_MAG1] == ELFMAG1 &&
        elf_header->e_ident[EI_MAG2] == ELFMAG2 &&
        elf_header->e_ident[EI_MAG3] == ELFMAG3) // valid elf magic bytes
        return executable_file_format::ELF; // type of elf

    // pe

    throw "no standard executable file format found";
}