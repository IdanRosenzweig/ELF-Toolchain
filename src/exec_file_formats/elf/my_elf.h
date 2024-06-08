#ifndef LOADER_MY_ELF_H
#define LOADER_MY_ELF_H

#include "../../abstract/utils/basic_file.h"
#include "../../abstract/utils/udata.h"

#include "basic_elf_file.h"
#include "elf.h"

#include <string>
#include <vector>
using namespace std;

struct elf_header {
    uint8_t magic0;
    uint8_t magic1;
    uint8_t magic2;
    uint8_t magic3;

    uint8_t class_type;
    uint8_t data_encoding;
    uint8_t os_abi;

    size_t type;
    size_t architecture;
    size_t version;
    size_t entry_addr;

    size_t header_sz;
    size_t flags;

    // regarding sections
    size_t section_table_offset;
    size_t section_table_ent_sz;
    size_t no_section;

    size_t string_section_index;

    // regarding program headers
    size_t program_header_table_offset;
    size_t program_header_table_ent_sz;
    size_t no_program_header;

};

struct elf_section {
    string name;
    size_t type;
    size_t flags;

    // offset and size of section in the file
    size_t offset_in_file;
    size_t size_in_file;
    udata data; // the actual data

    size_t additional_info;
    size_t alignment;

    size_t entry_size; // if section contains inner table
};

struct elf_segment {
    size_t type;
    size_t flags;

    // offset and size of segment in the file
    size_t offset_in_file;
    size_t size_in_file;
    udata data;

    // the required offset and size when loading to memory
    size_t offset_in_load_memory;
    size_t size_in_load_memory;

    size_t alignment;
};

struct my_elf_file {
    elf_header header;
    vector<elf_section> sections;
    vector<elf_segment> segments;
};

template <int CLASS>
my_elf_file parse_elf(const basic_elf_file<CLASS>& elf) {
    using elf_file = basic_elf_file<CLASS>;

    my_elf_file res;
    {
        typename elf_file::header* ptr = elf.get_header();
        res.header.magic0 = ptr->e_ident[0];
        res.header.magic1 = ptr->e_ident[1];
        res.header.magic2 = ptr->e_ident[2];
        res.header.magic3 = ptr->e_ident[3];

        res.header.class_type = ptr->e_ident[EI_CLASS];
        res.header.data_encoding = ptr->e_ident[EI_DATA];
        res.header.os_abi = ptr->e_ident[EI_OSABI];

        res.header.type = ptr->e_type;
        res.header.architecture = ptr->e_machine;
        res.header.version = ptr->e_version;
        res.header.entry_addr = ptr->e_entry;

        res.header.header_sz = ptr->e_ehsize;
        res.header.type = ptr->e_flags;
    }

    {
        for (int i = 0; i < elf.number_of_sections(); i++) {
            typename elf_file::section * ptr = elf.get_section_at_index_from_table(i);

            elf_section section;
            section.name = elf.get_string_at_offset_from_strtab((size_t) ptr->sh_name);
            section.type = ptr->sh_type;
            section.flags = ptr->sh_flags;

            section.offset_in_file = ptr->sh_offset;
            section.size_in_file = ptr->sh_size;
            section.data = udata(section.size_in_file, 0);
            memcpy(section.data.data(), elf.raw_file->offset_in_file(section.offset_in_file), section.size_in_file);

            section.additional_info = ptr->sh_info;
            section.alignment = ptr->sh_addralign;

            section.entry_size = ptr->sh_entsize;
        }
    }

    {
        for (int i = 0; i < elf.number_of_program_headers(); i++) {
            typename elf_file::segment * ptr = elf.get_program_header_at_index_from_table(i);

            elf_segment segment;
            segment.type = ptr->p_type;
            segment.flags = ptr->p_flags;

            segment.offset_in_file = ptr->sh_offset;
            segment.size_in_file = ptr->sh_size;
            segment.data = udata(segment.size_in_file, 0);
            memcpy(segment.data.data(), elf.raw_file->offset_in_file(segment.offset_in_file), segment.size_in_file);

            segment.offset_in_load_memory = ptr->p_vaddr;
            segment.offset_in_load_memory = ptr->p_memsz;

            segment.alignment = ptr->p_align;
        }
    }
}

#endif //LOADER_MY_ELF_H
