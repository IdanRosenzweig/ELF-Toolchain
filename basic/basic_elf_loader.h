#ifndef LOADER_BASIC_ELF_LOADER_H
#define LOADER_BASIC_ELF_LOADER_H

#include <dlfcn.h>
#include <queue>

#include "../exec_file_formats/elf/elf_file.h"
#include "../utils/stack.h"

template<int CLASS, typename ELF_FILE>
class basic_elf_loader {
    static_assert(std::is_base_of<elf_file<CLASS>, ELF_FILE>(), "elf file type must be baseclass of elf_file<CLASS>");

protected:
    virtual bool validate_elf(const ELF_FILE &elf) const = 0;

    virtual void jump_entry_elf(void *entry_addr, void *stack_addr, const ELF_FILE &file) const = 0;

    virtual ELF_FILE open_elf(
            const char *str) const = 0; // policy of ELF is that the path to the interpreter is given as null terminated string

    virtual stack allocate_stack_elf(const ELF_FILE &elf) const = 0;

    // policy of elf to use segments that need to be loaded to the memory address space
    virtual size_t load_segments_elf(const ELF_FILE &elf,
                                     size_t *load_min_addr) const = 0;// returns the base address of the load, and stores the minimum address used

    virtual size_t setup_stack(struct stack stack, const ELF_FILE &elf, size_t entry_addr, size_t interp_load_bias,
                               size_t load_min_addr) const = 0;

    virtual void relocate_rela(typename elf_file<CLASS>::addr reloc_offset, size_t reloc_type, size_t relocation_value,
                               ssize_t addend, typename elf_file<CLASS>::sym *sym, size_t the_load_bias) const = 0;

    virtual void relocate_rel(typename elf_file<CLASS>::addr reloc_offset, size_t reloc_type, size_t relocation_value,
                              typename elf_file<CLASS>::sym *sym, size_t the_load_bias) const = 0;

    virtual void call_init_array_func(void (*ptr)(),
                                      const ELF_FILE &elf) const = 0; // call a function found in .init_array
    virtual void call_init_func(void (*ptr)(),
                                const ELF_FILE &elf) const = 0; // call the function found in .init_array

    virtual std::vector<std::string> get_possible_search_prefixes() const = 0;

    void do_relocations(const ELF_FILE &elf, size_t the_load_bias) const {
        // find the DYNAMIC program header
        typename elf_file<CLASS>::segment *segment;
        for (size_t i = 0; i < elf.get_header()->e_phnum; i++) {
            segment = elf.get_program_header(
                    (size_t) elf.get_header()->e_phoff + (size_t) elf.get_header()->e_phentsize * i);

            if (segment->p_type == PT_DYNAMIC) break; // found the DYNAMIC program header
        }
        if (segment->p_type != PT_DYNAMIC) return; // no DYNAMIC program header


        // needed files
        std::vector<std::string> needed_files;

        // the appropriate string and symbol table
        bool strtab = false;
        size_t dynamic_strtab = 0;

        bool symtab = false;
        size_t dynamic_symtab = 0;
        size_t dynamic_symtab_entry_size = 0;

        // rela relocations
        bool rela = false;
        size_t rela_table_addr = 0;
        size_t rela_table_size = 0;
        size_t rela_entry_size = 0;

        // rel relocations
        bool rel = false;
        size_t rel_table_addr = 0;
        size_t rel_table_size = 0;
        size_t rel_entry_size = 0;

        // plt relocations
        bool plt = false;
        size_t plt_reloc_type = 0;
        size_t plt_reloc_table_addr = 0;
        size_t plt_reloc_table_size = 0;

        // init and fini
        bool init_func = false;
        size_t init = 0;

        bool fini_func = false;
        size_t fini = 0;

        bool init_array_func = false;
        size_t init_array = 0;
        size_t init_array_size = 0;

        bool fini_array_func = false;
        size_t fini_array = 0;
        size_t fini_array_size = 0;

        // rpath and runpath
        std::string rpath;
        std::string runpath;

        size_t _i = 0;
        while (true) {
            typename elf_file<CLASS>::dyn *curr =
                    elf.get_dyn_at_raw_offset((size_t) segment->p_offset + sizeof(typename elf_file<CLASS>::dyn) * _i);

            if (curr->d_tag == DT_NULL) break;

            size_t val = curr->d_un.d_val;

            switch (curr->d_tag) {
                case DT_INIT: {
                    init = val;
                    init_func = true;
                    break;
                }
                case DT_INIT_ARRAY: {
                    init_array = val;
                    init_array_func = true;
                    break;
                }
                case DT_INIT_ARRAYSZ: {
                    init_array_size = val;
                    break;
                }

                case DT_FINI: {
                    fini = val;
                    fini_func = true;
                    break;
                }
                case DT_FINI_ARRAY: {
                    fini_array = val;
                    fini_array_func = true;
                    break;
                }
                case DT_FINI_ARRAYSZ: {
                    fini_array_size = val;
                    break;
                }

                case DT_NEEDED: {
                    needed_files.push_back(elf.get_string_from_dynstr(val));
                    break;
                }

                case DT_STRTAB: {
                    dynamic_strtab = val;
                    strtab = true;
                    break;
                }
                case DT_SYMTAB: {
                    dynamic_symtab = val;
                    symtab = true;
                    break;
                }
                case DT_SYMENT: {
                    dynamic_symtab_entry_size = val;
                    break;
                }

                case DT_RELA: {
                    rela_table_addr = val;
                    rela = true;
                    break;
                }
                case DT_RELASZ: {
                    rela_table_size = val;
                    break;
                }
                case DT_RELAENT: {
                    rela_entry_size = val;
                    break;
                }

                case DT_REL: {
                    rel_table_addr = val;
                    rel = true;
                    break;
                }
                case DT_RELSZ: {
                    rel_table_size = val;
                    break;
                }
                case DT_RELENT: {
                    rel_entry_size = val;
                    break;
                }

                case DT_JMPREL: {
                    plt_reloc_table_addr = val;
                    plt = true;
                    break;
                }
                case DT_PLTRELSZ: {
                    plt_reloc_table_size = val;
                    break;
                }
                case DT_PLTREL: {
                    plt_reloc_type = val;
                    break;
                }

                case DT_RPATH: {
                    rpath = elf.get_string_from_dynstr(val);
                    break;
                }
                case DT_RUNPATH: {
                    runpath = elf.get_string_from_dynstr(val);
                    break;
                }

            }

            _i++;
        }

        if (!strtab) throw "no dynamic strtab";
        if (!symtab) throw "no dynamic symbol table";


        // opening all the needed_files files
        std::vector<std::string> possible_prefixes;
        if (!runpath.empty()) possible_prefixes.push_back(runpath + "/");
        if (!rpath.empty()) possible_prefixes.push_back(rpath + "/");
        for (const std::string &prefix: get_possible_search_prefixes()) {
            possible_prefixes.push_back(prefix);
        }

        std::vector<void *> needed_handles(needed_files.size());
        for (size_t i = 0; i < needed_files.size(); i++) {
            std::string file;
            bool opened = false;

            for (const std::string &prefix: possible_prefixes) {
                file = prefix + needed_files[i];

                dlerror();
                void *handler = dlopen(file.c_str(), RTLD_LAZY | RTLD_GLOBAL); // RTLD_NOW?
                if (handler != nullptr) {
                    needed_handles[i] = handler;
                    opened = true;
                    break;
                }
            }

            if (!opened) throw "can't open/find a certain library using dlopen()";
        }


        // the RELA relocations
        std::queue<typename elf_file<CLASS>::rela *> rela_relocs;

        if (rela)
            for (size_t i = 0; i < rela_table_size / rela_entry_size; i++)
                rela_relocs.push(
                        reinterpret_cast<elf_file<CLASS>::rela *>(the_load_bias + rela_table_addr +
                                                                  rela_entry_size * i));

        // the REL relocations
        std::queue<typename elf_file<CLASS>::rel *> rel_relocs;

        if (rel)
            for (size_t i = 0; i < rel_table_size / rel_entry_size; i++)
                rel_relocs.push(
                        reinterpret_cast<elf_file<CLASS>::rel *>(the_load_bias + rel_table_addr + rel_entry_size * i));

        // adding the PLT relocations
        if (plt) {
            if (plt_reloc_type == DT_RELA) {
                for (size_t i = 0;
                     i < plt_reloc_table_size / rela_entry_size; i++)
                    rela_relocs.push(reinterpret_cast<elf_file<CLASS>::rela *>(the_load_bias + plt_reloc_table_addr +
                                                                               rela_entry_size * i));
            } else if (plt_reloc_type == DT_REL) {
                for (size_t i = 0;
                     i < plt_reloc_table_size / rel_entry_size; i++)
                    rel_relocs.push(reinterpret_cast<elf_file<CLASS>::rel *>(the_load_bias + plt_reloc_table_addr +
                                                                             rel_entry_size * i));
            } else throw "weird plt relocations type";
        }

        // doing the RELA relocations
        while (!rela_relocs.empty()) {
            typename elf_file<CLASS>::rela *curr_rela = rela_relocs.front();
            rela_relocs.pop();

            typename elf_file<CLASS>::sym *sym = reinterpret_cast<elf_file<CLASS>::sym *>(
                    the_load_bias + dynamic_symtab +
                    dynamic_symtab_entry_size * elf_file<CLASS>::ELF_R_SYM(curr_rela->r_info));

            char *sym_name = reinterpret_cast<char *>(the_load_bias + dynamic_strtab + (size_t) sym->st_name);

            // find relocation value
            size_t relocation_value = 0;
            if (sym_name[0] != '\x00') {
                bool found = false;

                dlerror(); // reset last call's error
                void *sym_val = dlsym(RTLD_DEFAULT, sym_name);
                char *err = dlerror();
                if (err == nullptr) { // no error, found
                    relocation_value = (size_t) sym_val;
                    found = true;
                }

                if (!found) {
                    if (elf_file<CLASS>::ELF_ST_BIND(sym->st_info) != STB_WEAK) {
                        throw "no relocation found, FAILED\n";
                    }
                    // no relocation found, but symbol is weak
                }
            }

            // do the actual relocation
            typename elf_file<CLASS>::addr reloc_offset =
                    ((typename elf_file<CLASS>::addr) the_load_bias) + curr_rela->r_offset;

            ssize_t addend = curr_rela->r_addend;

            relocate_rela(reloc_offset, elf_file<CLASS>::ELF_R_TYPE(curr_rela->r_info), relocation_value, addend, sym,
                          the_load_bias);

        }

        // doing the REL relocations
        while (!rel_relocs.empty()) {
            typename elf_file<CLASS>::rel *curr_rel = rel_relocs.front();
            rel_relocs.pop();

            typename elf_file<CLASS>::sym *sym = reinterpret_cast<elf_file<CLASS>::sym *>(
                    the_load_bias + dynamic_symtab +
                    dynamic_symtab_entry_size * elf_file<CLASS>::ELF_R_SYM(curr_rel->r_info));

            char *sym_name = reinterpret_cast<char *>(the_load_bias + dynamic_strtab + (size_t) sym->st_name);

            // find relocation value
            size_t relocation_value = 0;
            if (sym_name[0] != '\x00') {
                bool found = false;

                dlerror(); // reset last call's error
                void *sym_val = dlsym(RTLD_DEFAULT, sym_name);
                char *err = dlerror();
                if (err == nullptr) { // no error, found
                    relocation_value = (size_t) sym_val;
                    found = true;
                }

                if (!found) {
                    if (elf_file<CLASS>::ELF_ST_BIND(sym->st_info) != STB_WEAK) {
                        throw "no relocation found, FAILED\n";
                    }
                    // no relocation found, but symbol is weak
                }
            }

            // do the actual relocation
            typename elf_file<CLASS>::addr reloc_offset =
                    ((typename elf_file<CLASS>::addr) the_load_bias) + curr_rel->r_offset;


            relocate_rel(reloc_offset, elf_file<CLASS>::ELF_R_TYPE(curr_rel->r_info), relocation_value, sym,
                         the_load_bias);

        }


        // closing all the needed_files files
        for (size_t i = 0; i < needed_handles.size(); i++) {
            if (dlclose(needed_handles[i]) != 0) throw "error when dlclose()";
        }


        // call the INIT and INIT_ARRAY functions?
        // initialization function
//        if (init_func) {
//            void (*func)() = (void (*)()) (the_load_bias + init);
//            call_init_func(func, elf);
//        }
//        if (init_array_func) {
//
//            void (**array)() = reinterpret_cast<void (**)()>(
//                    the_load_bias + init_array
//            );
//
//            for (int i = 0; i < init_array_size / sizeof(void (**)()); i++) {
//                void (*func)() = array[i];
//                call_init_array_func(func, elf);
//            }
//        }

    }

    virtual void exit(const ELF_FILE &elf) const = 0;

    void load_and_run_elf(const ELF_FILE &elf,
                          bool explicit_use_interp = false // explicitly use the interpreter specified in the elf's INTERP program header
    ) const {
        // validating the ELF
        if (!validate_elf(elf)) throw "elf file is not valid";

        // loading elf segments
        size_t load_min_addr; // absolute minimum address used
        size_t load_bias = // load bias
                load_segments_elf(elf, &load_min_addr);
        size_t entry_addr = load_bias + (size_t) elf.get_header()->e_entry; // program entry address

        size_t interp_load_min_addr = 0;
        size_t interp_load_bias = 0;
        size_t interp_entry_addr = 0;
        bool invoke_interp = false;
        if (explicit_use_interp) { // load the interpreter and use it

            for (size_t i = 0; i < elf.get_header()->e_phnum; i++) {
                typename elf_file<CLASS>::segment *segment = elf.get_program_header(
                        (size_t) elf.get_header()->e_phoff + (size_t) elf.get_header()->e_phentsize * i);

                if (segment->p_type != PT_INTERP) continue;

                // found interpreter
                invoke_interp = true;

                std::string interp_path = elf.get_string_at_raw_offset(segment->p_offset);
                ELF_FILE interp = open_elf(interp_path.c_str());

                interp_load_bias = load_segments_elf(interp, &interp_load_min_addr);
                interp_entry_addr = interp_load_bias + (size_t) interp.get_header()->e_entry;

                break;
            }

        } else { // handle the DYNAMIC program header (if exists)
            do_relocations(elf, load_bias);
        }

        // allocate stack for program
        struct stack stack = allocate_stack_elf(elf);

        // setup stack
        size_t stack_entry_point = setup_stack(stack, elf, entry_addr,
                                               interp_load_bias,
                                               load_min_addr); // load_bias instead of load_min_addr?


        if (invoke_interp) {
            jump_entry_elf((void *) interp_entry_addr, (void *) stack_entry_point, elf);
        } else {
            jump_entry_elf((void *) entry_addr, (void *) stack_entry_point, elf);
        }

        // call FINI and FINI_ARRAY now?

        exit(elf);
    }

};

#endif //LOADER_BASIC_ELF_LOADER_H
