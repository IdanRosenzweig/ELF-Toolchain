#ifndef LOADER_LINUX_X64_ELF_LOADER_H
#define LOADER_LINUX_X64_ELF_LOADER_H

#include "../../general_unix/elf/basic_unix_elf_loader.h"

// TO BE COMPILED TO X86_64 AND LINUX ABI
class linux_x64_elf_loader : public basic_unix_elf_loader<64> {
public:
    using elf_file = basic_elf_file<64>;

    bool validate_elf(elf_file &elf) const override;


    void relocate_rela(elf_file::addr reloc_offset, size_t reloc_type, size_t relocation_value, ssize_t addend,
                       elf_file::sym *sym, size_t the_load_bias) const override;

    void relocate_rel(elf_file::addr reloc_offset, size_t reloc_type, size_t relocation_value, elf_file::sym *sym,
                      size_t the_load_bias) const override;

    vector <string> get_possible_search_prefixes() const override;

    linux_x64_elf_loader();

};

#endif //LOADER_LINUX_X64_ELF_LOADER_H
