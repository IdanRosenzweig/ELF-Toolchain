#ifndef LOADER_UNIX_BASED_ELF_LOADER_H
#define LOADER_UNIX_BASED_ELF_LOADER_H

#include "include/mman.h"
#include "include/auxv.h"
#include "include/fcntl.h"
#include "include/unistd.h"
#include "include/error_codes.h"
#include <cstring>

#include "unix_based_loader.h"
#include "../basic/basic_elf_loader.h"
#include "opened_elf_file.h"

#include "../exec_file_formats/elf/elf_flags.h"

#include "../utils/macros.h"

template<int CLASS>
class unix_based_elf_loader
        : virtual public unix_based_loader<elf_flags>, public basic_elf_loader<CLASS, opened_elf<CLASS>> {
private:
    const size_t PAGE_SIZE; // required by elf auxiliary vector
    const char *PLATFORM; // required by elf auxiliary vector

    stack allocate_stack_elf(const opened_elf<CLASS> &elf) const override {
#define STACK_SIZE (0x1000000 * 0x10)
        size_t size = STACK_SIZE;

        int prot; // prot flags
        bool found = false;
        for (size_t i = 0; i < elf.get_header()->e_phnum; i++) {
            typename elf_file<CLASS>::segment *stack_segment = elf.get_program_header(
                    (size_t) elf.get_header()->e_phoff + (size_t) elf.get_header()->e_phentsize * i);

            if (stack_segment->p_type != PT_GNU_STACK) continue;

            found = true;
            prot = PROT_NONE;
            if (stack_segment->p_flags & PF_R) prot |= PROT_READ;
            if (stack_segment->p_flags & PF_W) prot |= PROT_WRITE;
            if (stack_segment->p_flags & PF_X) prot |= PROT_EXEC;

            break;
        }
        if (!found) prot = PROT_READ | PROT_WRITE;

        int flags = MAP_ANONYMOUS | MAP_PRIVATE;

        void *stack = mmap(nullptr, size, prot, flags, -1, 0);
        if (stack == MAP_FAILED) throw "failed to allocate stack";

        memset(stack, '\x00', size);

        return {stack, size, prot};
    }

    size_t load_segments_elf(const opened_elf<CLASS> &elf, size_t *load_min_addr) const override {

        // check program headers
        size_t segments_count = elf.get_header()->e_phnum;
        if (segments_count == 0) throw "no segments in the elf file";
        size_t segments_table_off = elf.get_header()->e_phoff;
        size_t segment_table_entry_size = elf.get_header()->e_phentsize;

        // checking that the process can hold the whole elf's image, calculating the load base address on the way
        // the segments are congruent
        size_t min = SIZE_MAX;
        size_t max = 0;
        for (size_t i = 0; i < segments_count; i++) {
            typename elf_file<CLASS>::segment *segment = elf.get_program_header(
                    segments_table_off + segment_table_entry_size * i);

            if (segment->p_type != PT_LOAD) continue;

            // raw load address and mapping length
            size_t raw_load_addr = (size_t) segment->p_vaddr;
            size_t raw_mapping_len = (size_t) segment->p_memsz;
//            if (raw_mapping_len == 0) continue;

            // calc adjusted load address and mapping length
            size_t adjusted_load_addr = ROUND_DOWN(raw_load_addr,
                                                   PAGE_SIZE); // aligned on page boundary (rounding down)
            size_t adjusted_mapping_len = ROUND_UP(raw_mapping_len + (raw_load_addr - adjusted_load_addr),
                                                   PAGE_SIZE); // aligned on page size (rounding up)

            if (adjusted_load_addr < min) min = adjusted_load_addr;
            if (max < adjusted_load_addr + adjusted_mapping_len) max = adjusted_load_addr + adjusted_mapping_len;
        }
        size_t range = max - min;

        size_t bias;
        if (elf.get_header()->e_type == ET_EXEC) {
            bias = 0;
            void *check = mmap((void *) min, range, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1,
                               0);
            if (check == MAP_FAILED) throw "mmap fail";
            munmap(check, range);
        } else { // ET_DYN
            void *mapped = mmap(nullptr, range, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if (mapped == MAP_FAILED) throw "mmap fail";
            munmap(mapped, range);

            bias = (size_t) mapped;
        }


        // mapping all the LOAD segments
        *load_min_addr = (size_t) SIZE_MAX; // min mapping address used
        for (size_t i = 0; i < segments_count; i++) {
            typename elf_file<CLASS>::segment *segment = elf.get_program_header(
                    segments_table_off + segment_table_entry_size * i);

            if (segment->p_type != PT_LOAD) continue; // checking segment type

            // raw load address and mapping length
            size_t raw_load_addr = (size_t) bias + (size_t) segment->p_vaddr;
            size_t raw_mapping_len = (size_t) segment->p_memsz;
//            if (raw_mapping_len == 0) continue;

            // calc adjusted load address and mapping length
            size_t adjusted_load_addr = ROUND_DOWN(raw_load_addr,
                                                   PAGE_SIZE); // aligned on page boundary (rounding down)
            size_t adjusted_mapping_len = ROUND_UP(raw_mapping_len + (raw_load_addr - adjusted_load_addr),
                                                   PAGE_SIZE); // aligned on page size (rounding up)


            // mapping
            void *mapped = mmap((void *) adjusted_load_addr, adjusted_mapping_len, PROT_WRITE,
                                MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            if (mapped == MAP_FAILED) throw "couldn't map the segment";


            // clearing the mapping additional mapping caused by alignment
//            memset(mapped, '\x00', adjusted_mapping_len - raw_mapping_len);

            // copy contents of segment (if exists)
            size_t content_len = segment->p_filesz;
            if (content_len > 0) // writing the segment's content
                memcpy((void *) raw_load_addr, elf.get_offset((size_t) segment->p_offset), segment->p_filesz);


            if (content_len < raw_mapping_len) // clearing the rest of the mapping (if exists)
                memset((void *) ((size_t) raw_load_addr + content_len), '\x00', raw_mapping_len - content_len);


            // change back protection
            int prot_flags = PROT_NONE;
            if (segment->p_flags & PF_R) prot_flags |= PROT_READ;
            if (segment->p_flags & PF_W) prot_flags |= PROT_WRITE;
            if (segment->p_flags & PF_X) prot_flags |= PROT_EXEC;
            mprotect(mapped, adjusted_mapping_len, prot_flags); // using adjusted values

            if (*load_min_addr == (size_t) SIZE_MAX || adjusted_load_addr < *load_min_addr)
                *load_min_addr = adjusted_load_addr;
        }

        return bias; // returns the base address of the load
    }

    size_t setup_stack(struct stack stack, const opened_elf<CLASS> &elf,
                       size_t entry_addr,
                       size_t interp_load_bias,
                       size_t load_min_addr) const override {
        size_t _stack_top = (size_t) stack.ptr + stack.size; // absolute stack top
        _stack_top = ROUND_DOWN(_stack_top, 0x10); // aligned on 16 byte (rounding down)

        size_t _stack_bottom = (size_t) stack.ptr; // absolute stack bottom
        _stack_bottom = ROUND_UP(_stack_bottom, 0x10); // aligned on 16 byte (rounding up)

#define MAX_STRINGS_SPACE 0x8000
        size_t reserve_start = ROUND_DOWN(_stack_top - MAX_STRINGS_SPACE, 0x10); // aligned on 16 byte (rounding down)
        size_t curr_reserve = reserve_start;

        // argc
        unsigned long *stack_argc = (unsigned long *) _stack_bottom;
        *stack_argc = proc_var.argc;
        stack_argc++;

        // argv
        char **stack_argv = (char **) stack_argc;
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
        typename elf_file<CLASS>::auxiliary *stack_aux = reinterpret_cast<elf_file<CLASS>::auxiliary *>(stack_env); // ROUND_UP((size_t) stack_env, 0x10) doesn't work for some reason

#define set_auxiliary_var(ptr, type, val) (ptr)->a_type = type; ptr->a_un.a_val = (uint64_t) (val); (ptr)++;
        set_auxiliary_var(stack_aux, AT_SYSINFO_EHDR, getauxval(AT_SYSINFO_EHDR));
        set_auxiliary_var(stack_aux, AT_MINSIGSTKSZ, getauxval(AT_MINSIGSTKSZ));
        set_auxiliary_var(stack_aux, AT_HWCAP, getauxval(AT_HWCAP)); // 0x178bfbff
        set_auxiliary_var(stack_aux, AT_PHDR, (size_t) (load_min_addr + elf.get_header()->e_phoff));
        set_auxiliary_var(stack_aux, AT_CLKTCK, getauxval(AT_CLKTCK));

        set_auxiliary_var(stack_aux, AT_PHENT, elf.get_header()->e_phentsize);
        set_auxiliary_var(stack_aux, AT_PHNUM, elf.get_header()->e_phnum);
        set_auxiliary_var(stack_aux, AT_PAGESZ, PAGE_SIZE);

        set_auxiliary_var(stack_aux, AT_BASE, interp_load_bias);
        set_auxiliary_var(stack_aux, AT_FLAGS, 0)
        set_auxiliary_var(stack_aux, AT_ENTRY, entry_addr);

        set_auxiliary_var(stack_aux, AT_UID, getuid());
        set_auxiliary_var(stack_aux, AT_EUID, geteuid());
        set_auxiliary_var(stack_aux, AT_GID, getgid());
        set_auxiliary_var(stack_aux, AT_EGID, getegid());

        set_auxiliary_var(stack_aux, AT_SECURE, getauxval(AT_SECURE));

        int fd = open("/dev/random", O_RDONLY);
        if (fd == OPEN_ERROR) throw "can't open /dev/random";
        if (read(fd, (void *) curr_reserve, 16) != 16) throw "can't read from /dev/random";
        if(close(fd) == CLOSE_ERROR) throw "can't close /dev/random";
        set_auxiliary_var(stack_aux, AT_RANDOM, curr_reserve);
        curr_reserve += 16;

        set_auxiliary_var(stack_aux, AT_HWCAP2, getauxval(AT_HWCAP2));

        size_t file_id_len = strlen(file_id) + 1; // including the trailing null byte
        memcpy((char *) curr_reserve, file_id, file_id_len);
        set_auxiliary_var(stack_aux, AT_EXECFN, (char *) curr_reserve);
        curr_reserve += file_id_len;

        size_t platform_len = strlen(PLATFORM) + 1; // including the trailing null byte
        memcpy((char *) curr_reserve, PLATFORM, platform_len);
        set_auxiliary_var(stack_aux, AT_PLATFORM, (char *) curr_reserve);
        curr_reserve += platform_len;

        set_auxiliary_var(stack_aux, AT_NULL, 0);

        // shifting up
        size_t sz = (size_t) stack_aux - (size_t) _stack_bottom; // size used by argc, argv, env, and auxiliary vector
        size_t stack_entry_point = ROUND_DOWN(reserve_start - sz, 0x10); // final stack entry point
        memmove((void *) stack_entry_point, (void *) _stack_bottom, sz); // shifting them up

        return stack_entry_point;
    }

    void call_init_array_func(void (*ptr)(), const opened_elf<64> &elf) const override {
        (*reinterpret_cast<void (*)(int, char **, char **)>(ptr))(proc_var.argc, proc_var.argv, proc_var.env);
    }

    void call_init_func(void (*ptr)(), const opened_elf<64> &elf) const override {
        (*reinterpret_cast<void (*)(int, char **, char **)>(ptr))(proc_var.argc, proc_var.argv, proc_var.env);
    }


protected:
    unix_based_elf_loader(size_t pageSize, const char *platform) : PAGE_SIZE(pageSize), PLATFORM(platform) {}

    opened_elf<CLASS> open_elf(const char *str) const override {
        opened_elf<CLASS> elf;
        elf.buff = open_file(str).buff; // copy the pointer

        return elf;
    }

    opened_elf<CLASS> cast_to_elf(const opened_unix_file &file) const {
        opened_elf<CLASS> elf;
        elf.buff = file.buff;
        return elf;
    }

public:
    unix_based_elf_loader() = delete;
};

#endif //LOADER_UNIX_BASED_ELF_LOADER_H
