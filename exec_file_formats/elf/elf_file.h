#ifndef LOADER_ELF_FILE_H
#define LOADER_ELF_FILE_H

#include "elf.h"

#include <type_traits>
#include <string>

template<int CLASS>
struct elf_file {
    static_assert(CLASS == 64 || CLASS == 32);

    typedef typename std::conditional<CLASS == 64, Elf64_Ehdr, Elf32_Ehdr>::type header;
    typedef typename std::conditional<CLASS == 64, Elf64_Phdr, Elf32_Phdr>::type segment;
    typedef typename std::conditional<CLASS == 64, Elf64_Shdr, Elf32_Shdr>::type section;
    typedef typename std::conditional<CLASS == 64, Elf64_auxv_t, Elf32_auxv_t>::type auxiliary;
    typedef typename std::conditional<CLASS == 64, Elf64_Dyn, Elf32_Dyn>::type dyn;
    typedef typename std::conditional<CLASS == 64, Elf64_Rel, Elf32_Rel>::type rel;
    typedef typename std::conditional<CLASS == 64, Elf64_Rela, Elf32_Rela>::type rela;
    typedef typename std::conditional<CLASS == 64, Elf64_Sym , Elf32_Sym >::type sym;
    typedef typename std::conditional<CLASS == 64, Elf64_Addr , Elf32_Addr>::type addr;
    typedef typename std::conditional<CLASS == 64, Elf64_Xword , Elf32_Xword>::type word;
    static auto ELF_R_SYM(auto x) {
        if constexpr (CLASS == 64) return ELF64_R_SYM(x);
        else if constexpr (CLASS == 32) return ELF32_R_SYM(x);
    }
    static auto ELF_R_TYPE(auto x) {
        if constexpr (CLASS == 64) return ELF64_R_TYPE(x);
        else if constexpr (CLASS == 32) return ELF32_R_TYPE(x);
    }
    static auto ELF_ST_BIND(auto x) {
        if constexpr (CLASS == 64) return ELF64_ST_BIND(x);
        else if constexpr (CLASS == 32) return ELF32_ST_BIND(x);
    }
    static auto ELF_ST_TYPE(auto x) {
        if constexpr (CLASS == 64) return ELF64_ST_TYPE(x);
        else if constexpr (CLASS == 32) return ELF32_ST_TYPE(x);
    }

    virtual header *get_header() const = 0;

    virtual segment *get_program_header(unsigned long long) const = 0;

    virtual section *get_section(unsigned long long) const = 0;

    virtual section *find_section(const char *name) const = 0;

    virtual dyn *get_dyn_at_raw_offset(unsigned long long) const = 0;

    virtual std::string get_string_at_raw_offset(unsigned long long off) const = 0;

    virtual std::string get_string_from_strtab(unsigned long long off) const = 0;

    virtual std::string get_string_from_dynstr(unsigned long long off) const = 0;

    virtual rel *get_rel_at_raw_offset(unsigned long long off) const = 0;

    virtual rela *get_rela_at_raw_offset(unsigned long long off) const = 0;

    virtual sym *get_sym_at_raw_offset(unsigned long long off) const = 0;

    virtual sym* find_sym_from_symtab(const char* name) const = 0;

    virtual sym* find_sym_from_dynsym(const char* name) const = 0;

};

#endif //LOADER_ELF_FILE_H
