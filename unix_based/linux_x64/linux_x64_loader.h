#ifndef LOADER_LINUX_X64_LOADER_H
#define LOADER_LINUX_X64_LOADER_H

#include "../unix_based_elf_loader.h"

// TO BE COMPILED TO X86_64 AND LINUX ABI
class linux_x64_loader : public unix_based_elf_loader<64> {
protected:
    bool validate_elf(const opened_elf<64> &elf) const override;

    void jump_entry_elf(void *entry_addr, void *stack_addr, const opened_elf<64>& elf) const override;

    void relocate_rela(typename elf_file<64>::addr reloc_offset, size_t reloc_type, size_t relocation_value, ssize_t addend, elf_file<64>::sym *sym, size_t the_load_bias) const override;

    void relocate_rel(typename elf_file<64>::addr reloc_offset, size_t reloc_type, size_t relocation_value, elf_file<64>::sym *sym, size_t the_load_bias) const override;

    std::vector<std::string> get_possible_search_prefixes() const override;

    void exit(const opened_elf<64> &elf) const override;

public:
    void load_and_run() const override;

    linux_x64_loader() : unix_based_elf_loader<64>(0x1000, "x86_64") {}
};

#endif //LOADER_LINUX_X64_LOADER_H
