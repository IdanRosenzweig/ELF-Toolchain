#ifndef ELF_TOOLCHAIN_ELF_OBFUSCATOR_H
#define ELF_TOOLCHAIN_ELF_OBFUSCATOR_H

#include "../basic_elf_file.h"

template <int CLASS>
void obfuscated_elf_file(basic_elf_file<CLASS>& elf) {
    // randomize segments
    random_shuffle(elf.parsed_segments.begin(), elf.parsed_segments.end());

    // randomize sections? need to remember the index of the strings table section
}


#endif //ELF_TOOLCHAIN_ELF_OBFUSCATOR_H
