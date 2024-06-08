#ifndef LOADER_ELF_PACKER_H
#define LOADER_ELF_PACKER_H

#include <cstring>
#include "../../../abstract/packing/basic_packed_file.h"
#include "../basic_elf_file.h"

template<int CLASS>
class elf_strip {
public:
    using elf_file = basic_elf_file<CLASS>;

    static void delete_elf_sections(const elf_file &elf) {
        // for each section:
        // delete its name from the string table
        // delete it from the sections table

        auto *strtab = elf.get_section_at_index_from_table(elf.get_header()->e_shstrndx); // (string table)

        for (size_t i = 0; i < elf.number_of_sections(); i++) {
            auto section = elf.get_section_at_index_from_table(i);

            // delete the name
            uint8_t *name = elf.raw_file->offset_in_file((size_t) strtab->sh_offset + (size_t) section->sh_name);
            memset(name, 0, strlen((char *) name));

            // delete the section itself from the table
            memset((void*) section, 0, elf.sz_of_section_table_entry());
        }

        // mark number of sections 0
        elf.number_of_sections() = 0;

        // delete the string section entry index
        elf.get_header()->e_shstrndx = 0;
    }

    unique_ptr<basic_file> strip_file(unique_ptr<basic_file> &&file) {
        elf_file elf(std::move(file));
        delete_elf_sections(elf);
        return std::move(elf.raw_file);
    }

};


#endif //LOADER_ELF_PACKER_H
