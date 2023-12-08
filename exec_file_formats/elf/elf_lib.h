#ifndef LOADER_ELF_LIB_H
#define LOADER_ELF_LIB_H

#include "elf_file.h"

template <int CLASS>
bool validate_elf_architecture(const typename elf_file<CLASS>::header *elf_header, uint16_t arc) {
    return elf_header->e_machine == arc;
}
template <int CLASS>
bool validate_elf_os(const typename elf_file<CLASS>::header *elf_header, unsigned char os) {
    return elf_header->e_ident[EI_OSABI] == os;
}

template <int CLASS>
bool validate_elf_type(const typename elf_file<CLASS>::header *elf_header, uint16_t type) {
    return elf_header->e_type == type;
}

template <int CLASS>
bool validate_elf_version(const typename elf_file<CLASS>::header *elf_header) {
    return elf_header->e_version == EV_CURRENT;
}

#endif //LOADER_ELF_LIB_H
