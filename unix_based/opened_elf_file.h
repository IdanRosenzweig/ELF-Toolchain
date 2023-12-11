#ifndef LOADER_OPENED_ELF_FILE_H
#define LOADER_OPENED_ELF_FILE_H

#include "opened_unix_file.h"
#include "../exec_file_formats/elf/elf_file.h"

template<int CLASS>
struct opened_elf : public opened_unix_file, public elf_file<CLASS> {
public:
    elf_file<CLASS>::header *get_header() const override {
        return reinterpret_cast<elf_file<CLASS>::header *>(buff);
    }

    elf_file<CLASS>::segment *get_program_header(unsigned long long off) const override {
        return reinterpret_cast<elf_file<CLASS>::segment *>(get_offset(off));
    }

    elf_file<CLASS>::section *get_section(unsigned long long off) const override {
        return reinterpret_cast<elf_file<CLASS>::section *>(get_offset(off));
    }

    elf_file<CLASS>::section *find_section(const char *name) const override {
        size_t start = get_header()->e_shoff;
        size_t entsz = get_header()->e_shentsize;

        for (size_t i = 0; i < get_header()->e_shnum; i++) {
            size_t curr_sec_off(
                    start + // start of section headers
                    entsz * i // current entry
            );

            typename elf_file<CLASS>::section *section = get_section(curr_sec_off);

            std::string sec_name = get_string_from_strtab((size_t) section->sh_name);

            if (sec_name == name)
                return reinterpret_cast<elf_file<CLASS>::section *>(section);
        }

        return nullptr;
    }

    elf_file<CLASS>::dyn *get_dyn_at_raw_offset(unsigned long long off) const override {
        return reinterpret_cast<elf_file<CLASS>::dyn *>(get_offset(off));
    }

    std::string get_string_at_raw_offset(unsigned long long off) const override {
        // building the string
        std::string str;

        char *ptr = (char *) get_offset(off);

        size_t i = 0;
        while (ptr[i] != '\x00') {
            str += ptr[i];
            i++;
        }

        return str;
    }

    std::string get_string_from_strtab(unsigned long long off) const override {
        // the string table section
        typename elf_file<CLASS>::section *strtab = get_section(
                (size_t) get_header()->e_shoff + // start of section headers
                ((size_t) get_header()->e_shentsize) *
                ((size_t) get_header()->e_shstrndx) // jumping to strtab entry
        );

        return get_string_at_raw_offset((size_t) strtab->sh_offset + off);
    }

    std::string get_string_from_dynstr(unsigned long long off) const override {
        typename elf_file<CLASS>::section *dynstr = find_section(".dynstr");

        return get_string_at_raw_offset((size_t) dynstr->sh_offset + off);
    }

    elf_file<CLASS>::rel *get_rel_at_raw_offset(unsigned long long off) const override {
        return reinterpret_cast<elf_file<CLASS>::rel *>(get_offset(off));
    }

    elf_file<CLASS>::rela *get_rela_at_raw_offset(unsigned long long off) const override {
        return reinterpret_cast<elf_file<CLASS>::rela *>(get_offset(off));
    }

    elf_file<CLASS>::sym *get_sym_at_raw_offset(unsigned long long off) const override {
        return reinterpret_cast<elf_file<CLASS>::sym *>(get_offset(off));
    }

    elf_file<CLASS>::sym *find_sym_from_symtab(const char *name) const override {
        typename elf_file<CLASS>::section *symtab = find_section(".symtab");

        if (symtab == nullptr) return nullptr;

        for (size_t i = 0; i < symtab->sh_size / symtab->sh_entsize; i++) {
            typename elf_file<CLASS>::sym *sym = get_sym_at_raw_offset(symtab->sh_offset + symtab->sh_entsize * i);

            std::string sym_name = get_string_from_dynstr(sym->st_name); // the symbol name

            if (sym_name == name) return sym;
        }

        return nullptr;
    }

    elf_file<CLASS>::sym *find_sym_from_dynsym(const char *name) const override {
        typename elf_file<CLASS>::section *dynsym = find_section(".dynsym");

        if (dynsym == nullptr) return nullptr;

        for (size_t i = 0; i < dynsym->sh_size / dynsym->sh_entsize; i++) {
            typename elf_file<CLASS>::sym *sym = get_sym_at_raw_offset(dynsym->sh_offset + dynsym->sh_entsize * i);

            std::string sym_name = get_string_from_dynstr(sym->st_name); // the symbol name

            if (sym_name == name) return sym;
        }

        return nullptr;
    }

    elf_file<CLASS>::verdef * get_verdef_at_raw_offset(unsigned long long off) const override {
        return reinterpret_cast<elf_file<CLASS>::verdef *>(get_offset(off));
    }
};

#endif //LOADER_OPENED_ELF_FILE_H
