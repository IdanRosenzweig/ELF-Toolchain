#ifndef LOADER_BASIC_UNIX_ELF_LOADER_H
#define LOADER_BASIC_UNIX_ELF_LOADER_H

#include <sys/mman.h>
#include <sys/auxv.h>
#include <sys/fcntl.h>
#include <linux/unistd.h>
#include "error_codes.h"

#include <cstring>
#include <unistd.h>

using namespace std;

#include "../basic/loader/elf/basic_elf_loader.h"
#include "opened_unix_file.h"

#include "../basic/utils/macros.h"
#include "../basic/utils/process_vars.h"

template<int CLASS>
class basic_unix_elf_loader : public basic_elf_loader<CLASS> {
public:
    using elf_file = basic_elf_file<CLASS>;


    elf_file open_elf(const string &path) const override {
        return elf_file(make_unique<opened_unix_file>(path));
    }

    virtual bool validate_elf(const elf_file &elf) const override = 0;


    void *map_fixed(void *addr, size_t len) const override {
        void *mapped = mmap(addr, len, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
        if (mapped == MAP_FAILED) return ELF_MAP_ERROR;
        else return mapped;
    }

    void *map_arbitrary(size_t len) const override {
        void *mapped = mmap(nullptr, len, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (mapped == MAP_FAILED) return ELF_MAP_ERROR;
        else return mapped;
    }

    int unmap(void *addr, size_t len) const override {
        if (munmap(addr, len) == MUNMAP_ERROR) return ELF_UNMAP_ERROR;
        else return 0;
    }

    int protect(void *addr, size_t len, int flags) const override {
        int prot_flags = PROT_NONE;
        if (flags & PF_R) prot_flags |= PROT_READ;
        if (flags & PF_W) prot_flags |= PROT_WRITE;
        if (flags & PF_X) prot_flags |= PROT_EXEC;

        if (mprotect(addr, len, prot_flags) == MPROTECT_ERROR) return ELF_MAP_PROTECT_ERROR;
        else return 0;
    }


    stack allocate_stack(const elf_file &elf, size_t stack_sz = 0x1000000 * 0x10) const override {
        // searching for the extension PT_GNU_STACK program header
        bool found = false;
        int flags; // prot flags
        for (size_t i = 0; i < elf.number_of_program_headers(); i++) {
            typename elf_file::segment *stack_segment = elf.get_program_header_at_index_from_table(i);

            if (stack_segment->p_type != PT_GNU_STACK) continue;

            found = true;
            flags = PROT_NONE;
            if (stack_segment->p_flags & PF_R) flags |= PROT_READ;
            if (stack_segment->p_flags & PF_W) flags |= PROT_WRITE;
            if (stack_segment->p_flags & PF_X) flags |= PROT_EXEC;

            break;
        }

        if (!found) { // if no extension was found, allocate the stack in the regular way
            return this->basic_elf_loader<CLASS>::allocate_stack(elf);
        }

        void *stack = map_arbitrary(stack_sz);
        if (stack == ELF_MAP_ERROR) throw "failed to allocate stack";

        // clearing the stack
        memset(stack, '\x00', stack_sz);

        // setting read and write protection on the stack
        if (protect(stack, stack_sz, flags) == ELF_MAP_PROTECT_ERROR) throw "failed to change stack flags";

        return {stack, stack_sz};
    }

    size_t setup_stack(struct stack stack, const elf_file &elf, size_t entry_addr, size_t interp_load_bias,
                       size_t load_min_addr) const override {
        size_t _stack_top = (size_t) stack.ptr + stack.size; // absolute stack top
        _stack_top = ROUND_DOWN(_stack_top, 0x10); // aligned on 16 byte (rounding down)

        size_t _stack_bottom = (size_t) stack.ptr; // absolute stack bottom
        _stack_bottom = ROUND_UP(_stack_bottom, 0x10); // aligned on 16 byte (rounding up)

#define MAX_STRINGS_SPACE 0x8000
        size_t reserve_start = ROUND_DOWN(_stack_top - MAX_STRINGS_SPACE, 0x10); // aligned on 16 byte (rounding down)
        size_t curr_reserve = reserve_start;

        // argc
        unsigned long *stack_argc = reinterpret_cast<unsigned long *>(_stack_bottom);
        *stack_argc = proc_var.argc;
        stack_argc++;

        // argv
        char **stack_argv = reinterpret_cast<char **>(stack_argc);
        for (size_t i = 0; proc_var.argv[i] != nullptr; i++) {
            size_t len = strlen(proc_var.argv[i]) + 1; // including the trailing null byte

            memcpy((char *) curr_reserve, proc_var.argv[i], len);
            *stack_argv = (char *) curr_reserve;

            stack_argv++;
            curr_reserve += len;
        }
        *stack_argv = nullptr;
        stack_argv++;

        // env
        char **stack_env = stack_argv;
        for (size_t i = 0; proc_var.env[i] != nullptr; i++) {
            size_t len = strlen(proc_var.env[i]) + 1; // including the trailing null byte

            memcpy((char *) curr_reserve, proc_var.env[i], len);
            *stack_env = (char *) curr_reserve;

            stack_env++;
            curr_reserve += len;
        }
        *stack_env = nullptr;
        stack_env++;

        // setup auxiliary vector variables
        typename elf_file::auxiliary *stack_aux = reinterpret_cast<elf_file::auxiliary *>(stack_env);

#define set_auxiliary_var(ptr, type, val) (ptr)->a_type = type; ptr->a_un.a_val = (uint64_t) (val); (ptr)++;
        set_auxiliary_var(stack_aux, AT_SYSINFO, getauxval(AT_SYSINFO));
        set_auxiliary_var(stack_aux, AT_SYSINFO_EHDR, getauxval(AT_SYSINFO_EHDR));
        set_auxiliary_var(stack_aux, AT_MINSIGSTKSZ, getauxval(AT_MINSIGSTKSZ));
        set_auxiliary_var(stack_aux, AT_HWCAP, getauxval(AT_HWCAP));
        set_auxiliary_var(stack_aux, AT_HWCAP2, getauxval(AT_HWCAP2));
        set_auxiliary_var(stack_aux, AT_PHDR, (size_t) (load_min_addr + elf.get_header()->e_phoff));
        set_auxiliary_var(stack_aux, AT_CLKTCK, getauxval(AT_CLKTCK));

        set_auxiliary_var(stack_aux, AT_PHENT, elf.sz_of_program_header_table_entry());
        set_auxiliary_var(stack_aux, AT_PHNUM, elf.number_of_program_headers());
        set_auxiliary_var(stack_aux, AT_PAGESZ, unix_specific_page_sz);

        set_auxiliary_var(stack_aux, AT_BASE, interp_load_bias);
        set_auxiliary_var(stack_aux, AT_FLAGS, 0)
        set_auxiliary_var(stack_aux, AT_ENTRY, entry_addr);

        set_auxiliary_var(stack_aux, AT_UID, getuid());
        set_auxiliary_var(stack_aux, AT_EUID, geteuid());
        set_auxiliary_var(stack_aux, AT_GID, getgid());
        set_auxiliary_var(stack_aux, AT_EGID, getegid());

        set_auxiliary_var(stack_aux, AT_DCACHEBSIZE, getauxval(AT_DCACHEBSIZE));
        set_auxiliary_var(stack_aux, AT_ICACHEBSIZE, getauxval(AT_ICACHEBSIZE));
        set_auxiliary_var(stack_aux, AT_UCACHEBSIZE, getauxval(AT_UCACHEBSIZE));

        set_auxiliary_var(stack_aux, AT_L1I_CACHESIZE, getauxval(AT_L1I_CACHESIZE));
        set_auxiliary_var(stack_aux, AT_L1I_CACHEGEOMETRY, getauxval(AT_L1I_CACHEGEOMETRY));
        set_auxiliary_var(stack_aux, AT_L1D_CACHESIZE, getauxval(AT_L1D_CACHESIZE));
        set_auxiliary_var(stack_aux, AT_L1D_CACHEGEOMETRY, getauxval(AT_L1D_CACHEGEOMETRY));
        set_auxiliary_var(stack_aux, AT_L2_CACHESIZE, getauxval(AT_L2_CACHESIZE));
        set_auxiliary_var(stack_aux, AT_L2_CACHEGEOMETRY, getauxval(AT_L2_CACHEGEOMETRY));
        set_auxiliary_var(stack_aux, AT_L3_CACHESIZE, getauxval(AT_L3_CACHESIZE));
        set_auxiliary_var(stack_aux, AT_L3_CACHEGEOMETRY, getauxval(AT_L3_CACHEGEOMETRY));

        set_auxiliary_var(stack_aux, AT_SECURE, getauxval(AT_SECURE));

#define RAND_ENDPOINT "/dev/random"
        int fd = open(RAND_ENDPOINT, O_RDONLY);
        if (fd == OPEN_ERROR) throw "can't open raw_file for random bytes";
        if (read(fd, (void *) curr_reserve, 16) != 16) throw "can't read random bytes";
        if (close(fd) == CLOSE_ERROR) throw "can't close raw_file of random bytes";
        set_auxiliary_var(stack_aux, AT_RANDOM, curr_reserve);
        curr_reserve += 16;

        set_auxiliary_var(stack_aux, AT_HWCAP2, getauxval(AT_HWCAP2));

        const string &path = static_cast<opened_unix_file *>(elf.raw_file.get())->path;
        size_t file_id_len = path.size() + 1; // including the trailing null byte
        memcpy((char *) curr_reserve, path.c_str(), file_id_len);
        set_auxiliary_var(stack_aux, AT_EXECFN, (char *) curr_reserve);
        curr_reserve += file_id_len;

        int platform_len = unix_specific_platform.size() + 1; // including the trailing null byte
        memcpy((char *) curr_reserve, unix_specific_platform.c_str(), platform_len);
        set_auxiliary_var(stack_aux, AT_PLATFORM, (char *) curr_reserve);
        curr_reserve += platform_len;

        set_auxiliary_var(stack_aux, AT_NULL, 0);

        // shifting up
        size_t sz = (size_t) stack_aux - (size_t) _stack_bottom; // size used by argc, argv, env, and auxiliary vector
        size_t stack_entry_point = ROUND_DOWN(reserve_start - sz, 0x10); // final stack entry point
        memmove((void *) stack_entry_point, (void *) _stack_bottom, sz); // shifting them up

        return stack_entry_point;
    }


    virtual void relocate_rela(elf_file::addr reloc_offset, size_t reloc_type, size_t relocation_value, ssize_t addend,
                               elf_file::sym *sym, size_t the_load_bias) const override = 0;

    virtual void
    relocate_rel(elf_file::addr reloc_offset, size_t reloc_type, size_t relocation_value, elf_file::sym *sym,
                 size_t the_load_bias) const override = 0;


    virtual void call_init_array_func(void (*ptr)(), const elf_file &elf) const override {
        (*reinterpret_cast<void (*)(int, char **, char **)>(ptr))(proc_var.argc, proc_var.argv, proc_var.env);
    }

    void call_init_func(void (*ptr)(), const elf_file &elf) const override {
        (*reinterpret_cast<void (*)(int, char **, char **)>(ptr))(proc_var.argc, proc_var.argv, proc_var.env);
    }

    virtual void jump_entry_elf(void *entry_addr, void *stack_addr, const elf_file &file) const override = 0;

    void exit(const elf_file &elf) const override {
        ::exit(0);
    }


    virtual vector<string> get_possible_search_prefixes() const override = 0;


    /** construct the specific unix parameters */
    basic_unix_elf_loader() = delete;

    basic_unix_elf_loader(const string &unixSpecificPlatform, size_t unixSpecificPageSz) : unix_specific_platform(
            unixSpecificPlatform), unix_specific_page_sz(unixSpecificPageSz) {}

private:
    const string unix_specific_platform;
    const size_t unix_specific_page_sz;

    // process variables
    struct process_vars proc_var;
public:
    const struct process_vars &getProcVar() const {
        return proc_var;
    }

    void setProcVar(const struct process_vars &procVar) {
        proc_var = procVar;
    }

};

#endif //LOADER_BASIC_UNIX_ELF_LOADER_H
