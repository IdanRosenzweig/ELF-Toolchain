#ifndef LOADER_AND_PACKER_STRIP_ELF_H
#define LOADER_AND_PACKER_STRIP_ELF_H

#include "../basic_elf_file.h"

template<int CLASS>
void strip_elf_file(basic_elf_file<CLASS> &elf) {
    // removing all unnecessary sections
    vector<typename basic_elf_file<CLASS>::section> needed_sections;

    for (int i = 0; i < elf.parsed_sections.size(); i++) {
        auto type = elf.parsed_sections[i].sh_type;

        switch (type) {
            case SHT_PROGBITS:
            case SHT_INIT_ARRAY:
            case SHT_FINI_ARRAY:
            case SHT_STRTAB: // strings table
            case SHT_SYMTAB: // symbols table
            {
                needed_sections.push_back(elf.parsed_sections[i]);
                if (i == elf.parsed_header.e_shstrndx)
                    elf.parsed_header.e_shstrndx = needed_sections.size() - 1;
            }
        }

        if (elf.parsed_header.e_type == ET_DYN)
            // dynamic linking related sections
            switch (type) {
                case SHT_REL:
                case SHT_RELA:
                case SHT_DYNAMIC:
                case SHT_DYNSYM:
                    needed_sections.push_back(elf.parsed_sections[i]);
            }
    }

    elf.parsed_sections = needed_sections;


    // removing all unnecessary segments
    vector<typename basic_elf_file<CLASS>::segment> needed_segments;

    for (int i = 0; i < elf.parsed_segments.size(); i++) {
        auto type = elf.parsed_segments[i].p_type;

        switch (type) {
            case PT_LOAD: // regular load segment
            case PT_GNU_STACK: // stack info
//            case PT_INTERP: // interpreter to use
                needed_segments.push_back(elf.parsed_segments[i]);
        }

        if (elf.parsed_header.e_type == ET_DYN)
            // dynamic linking related stuff
            switch (type) {
                case PT_DYNAMIC:
                    needed_segments.push_back(elf.parsed_segments[i]);
            }
    }

    elf.parsed_segments = needed_segments;

}

#endif //LOADER_AND_PACKER_STRIP_ELF_H
