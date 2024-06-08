#ifndef LOADER_BASIC_ELF_FILE_H
#define LOADER_BASIC_ELF_FILE_H

#include "../../abstract/utils/basic_file.h"
#include "../../abstract/utils/udata.h"

#include "elf.h"
#include <type_traits>
#include <string>
#include <memory>
#include <random>

using namespace std;

template<int CLASS>
struct basic_elf_file {
    static_assert(CLASS
    == 64 || CLASS == 32);

    typedef typename conditional<CLASS == 64, Elf64_Half, Elf32_Half>::type architecture_type;
    typedef typename conditional<CLASS == 64, Elf64_Half, Elf32_Half>::type object_type;
    typedef typename conditional<CLASS == 64, Elf64_Word, Elf32_Word>::type object_version;
    typedef typename conditional<CLASS == 64, Elf64_Addr, Elf32_Addr>::type entry_addr;

    typedef typename conditional<CLASS == 64, Elf64_Ehdr, Elf32_Ehdr>::type header;
    typedef typename conditional<CLASS == 64, Elf64_Phdr, Elf32_Phdr>::type segment;
    typedef typename conditional<CLASS == 64, Elf64_Shdr, Elf32_Shdr>::type section;
    typedef typename conditional<CLASS == 64, Elf64_auxv_t, Elf32_auxv_t>::type auxiliary;
    typedef typename conditional<CLASS == 64, Elf64_Dyn, Elf32_Dyn>::type dyn;
    typedef typename conditional<CLASS == 64, Elf64_Rel, Elf32_Rel>::type rel;
    typedef typename conditional<CLASS == 64, Elf64_Rela, Elf32_Rela>::type rela;
    typedef typename conditional<CLASS == 64, Elf64_Sym, Elf32_Sym>::type sym;

    static auto ELF_R_SYM(auto x) {
        if constexpr(CLASS == 64)
        return ELF64_R_SYM(x);
        else if constexpr(CLASS == 32)
        return ELF32_R_SYM(x);
    }

    static auto ELF_R_TYPE(auto x) {
        if constexpr(CLASS == 64)
        return ELF64_R_TYPE(x);
        else if constexpr(CLASS == 32)
        return ELF32_R_TYPE(x);
    }

    static auto ELF_ST_BIND(auto x) {
        if constexpr(CLASS == 64)
        return ELF64_ST_BIND(x);
        else if constexpr(CLASS == 32)
        return ELF32_ST_BIND(x);
    }

    static auto ELF_ST_TYPE(auto x) {
        if constexpr(CLASS == 64)
        return ELF64_ST_TYPE(x);
        else if constexpr(CLASS == 32)
        return ELF32_ST_TYPE(x);
    }

    typedef typename conditional<CLASS == 64, Elf64_Addr, Elf32_Addr>::type addr;
    typedef typename conditional<CLASS == 64, Elf64_Xword, Elf32_Xword>::type word;
    typedef typename conditional<CLASS == 64, Elf32_Verneed, Elf32_Verneed>::type verneed;
    typedef typename conditional<CLASS == 64, Elf64_Vernaux, Elf32_Vernaux>::type vernaux;
    typedef typename conditional<CLASS == 64, Elf64_Versym, Elf32_Versym>::type versym;


    static bool check_elf_magic(const unique_ptr<basic_file> &file) {
        header *elf_header = reinterpret_cast<header *>((*file).offset_in_file(0));
        return elf_header->e_ident[EI_MAG0] == ELFMAG0 &&
                elf_header->e_ident[EI_MAG1] == ELFMAG1 &&
                elf_header->e_ident[EI_MAG2] == ELFMAG2 &&
                elf_header->e_ident[EI_MAG3] == ELFMAG3;
    }

    /** header related */
    inline header *get_header() const {
        return reinterpret_cast<header *>(raw_file->offset_in_file(0));
    }

    uint8_t get_os_type() const {
        return get_header()->e_ident[EI_OSABI];
    }

    object_type get_object_type() const {
        return get_header()->e_type;
    }

    architecture_type get_architecture_type() const {
        return get_header()->e_machine;
    }

    object_version get_version() const {
        return get_header()->e_version;
    }

    entry_addr get_entry_addr() const {
        return get_header()->e_entry;
    }

    // sections
    inline auto &offset_of_sections_table() const {
        return get_header()->e_shoff;
    }

    inline auto &sz_of_section_table_entry() const {
        return get_header()->e_shentsize;
    }

    inline auto &number_of_sections() const {
        return get_header()->e_shnum;
    }

    // program headers
    inline auto &offset_of_program_headrs_table() const {
        return get_header()->e_phoff;
    }

    inline auto &sz_of_program_header_table_entry() const {
        return get_header()->e_phentsize;
    }

    inline auto &number_of_program_headers() const {
        return get_header()->e_phnum;
    }


    /** sections related */
    section *get_section_at_offset(size_t off) const {
        return reinterpret_cast<section *>(raw_file->offset_in_file(off));
    }

    section *get_section_at_index_from_table(size_t index) const {
        return reinterpret_cast<section *>(raw_file->offset_in_file(
                offset_of_sections_table() + sz_of_section_table_entry() * index));
    }

    section *find_section(const string &name) const {
        for (size_t i = 0; i < number_of_sections(); i++) {
            auto section = get_section_at_index_from_table(i);

            string sec_name = get_string_at_offset_from_strtab((size_t) section->sh_name);
            if (sec_name == name)
                return reinterpret_cast<basic_elf_file<CLASS>::section *>(section);
        }

        return nullptr;
    }


    /** program headers related */
    segment *get_program_header_at_offset(size_t off) const {
        return reinterpret_cast<segment *>(raw_file->offset_in_file(off));
    }

    segment *get_program_header_at_index_from_table(size_t index) const {
        return reinterpret_cast<segment *>(raw_file->offset_in_file(
                offset_of_program_headrs_table() + sz_of_program_header_table_entry() * index));
    }

    /** strings related */
    string get_string_at_offset(size_t off) const {
        // building the string
        string str;

        char *ptr = (char *) raw_file->offset_in_file(off);

        size_t i = 0;
        while (ptr[i] != '\x00') {
            str += ptr[i];
            i++;
        }

        return str;
    }

    string get_string_at_offset_from_strtab(size_t off) const {
        // the string table section
        section *strtab = get_section_at_index_from_table(get_header()->e_shstrndx);

        return get_string_at_offset((size_t) strtab->sh_offset + off);
    }

    string get_string_at_offset_from_dynstr(size_t off) const {
        section *dynstr = find_section(".dynstr");

        return get_string_at_offset((size_t) dynstr->sh_offset + off);
    }


    /** relocations related */
    rel *get_rel_at_offset(size_t off) const {
        return reinterpret_cast<rel *>(raw_file->offset_in_file(off));
    }

    rela *get_rela_at_offset(size_t off) const {
        return reinterpret_cast<rela *>(raw_file->offset_in_file(off));
    }

    /** symbols related */
    sym *get_sym_at_raw_offset(size_t off) const {
        return reinterpret_cast<sym *>(raw_file->offset_in_file(off));
    }

    sym *find_sym_from_symtab(const string &name) const {
        section *symtab = find_section(".symtab");
        if (symtab == nullptr) return nullptr;

        for (size_t i = 0; i < symtab->sh_size / symtab->sh_entsize; i++) {
            sym *sym = get_sym_at_raw_offset(symtab->sh_offset + symtab->sh_entsize * i);

            string sym_name = get_string_at_offset_from_dynstr(sym->st_name); // the symbol name

            if (sym_name == name) return sym;
        }

        return nullptr;
    }

    sym *find_sym_from_dynsym(const string &name) const {
        section *dynsym = find_section(".dynsym");
        if (dynsym == nullptr) return nullptr;

        for (size_t i = 0; i < dynsym->sh_size / dynsym->sh_entsize; i++) {
            sym *sym = get_sym_at_raw_offset(dynsym->sh_offset + dynsym->sh_entsize * i);

            string sym_name = get_string_at_offset_from_dynstr(sym->st_name); // the symbol name

            if (sym_name == name) return sym;
        }

        return nullptr;
    }

    dyn *get_dyn_at_raw_offset(size_t off) const {
        return reinterpret_cast<dyn *>(raw_file->offset_in_file(off));
    }


    /** construct from some base file */
    basic_elf_file() = delete;

    explicit basic_elf_file(unique_ptr <basic_file> &&file) : raw_file(std::move(file)) {}

    unique_ptr <basic_file> raw_file;


};

#endif //LOADER_BASIC_ELF_FILE_H
