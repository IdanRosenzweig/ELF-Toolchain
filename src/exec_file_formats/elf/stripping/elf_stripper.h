#ifndef ELF_TOOLCHAIN_STRIP_ELF_H
#define ELF_TOOLCHAIN_STRIP_ELF_H

#include "../basic_elf_file.h"

template<int CLASS>
void strip_elf_file(basic_elf_file<CLASS> &elf) {
    // removing all unnecessary sections
    vector<typename basic_elf_file<CLASS>::section> needed_sections;

    for (int i = 0; i < elf.parsed_sections.size(); i++) {
        auto type = elf.parsed_sections[i].sh_type;

        bool needed = false;

        switch (type) {
            case SHT_STRTAB: // strings table
            case SHT_SYMTAB: // symbols table
                needed = true;
        }

        if (elf.parsed_header.e_type == ET_DYN)
            // dynamic linking related sections
            switch (type) {
                case SHT_REL:
                case SHT_RELA:
                case SHT_DYNAMIC:
                case SHT_DYNSYM:
                    needed = true;
            }

        if (needed) {
            needed_sections.push_back(elf.parsed_sections[i]);

            if (i == elf.parsed_header.e_shstrndx)
                elf.parsed_header.e_shstrndx = needed_sections.size() - 1;
        }
    }

    elf.parsed_sections = needed_sections;
    elf.parsed_header.e_shnum = needed_sections.size();


    // removing all unnecessary segments
    vector<typename basic_elf_file<CLASS>::segment> needed_segments;

    for (int i = 0; i < elf.parsed_segments.size(); i++) {
        auto type = elf.parsed_segments[i].p_type;

        bool needed = false;

        switch (type) {
            case PT_LOAD: // regular load segment
            case PT_GNU_STACK: // stack info
//            case PT_INTERP: // interpreter to use
                needed = true;
        }

        if (elf.parsed_header.e_type == ET_DYN)
            // dynamic linking related stuff
            switch (type) {
                case PT_DYNAMIC:
                    needed = true;
            }

        if (needed) {
            needed_segments.push_back(elf.parsed_segments[i]);
        }
    }

    elf.parsed_segments = needed_segments;
    elf.parsed_header.e_phnum = needed_segments.size();

}

#endif //ELF_TOOLCHAIN_STRIP_ELF_H
