#include "linux_x64_loader.h"
#include "../../exec_file_formats/elf/elf_lib.h"

void jump_entry(volatile void *exec_entry, volatile void *stack_entry) {
    asm(
// exec_entry is in RDI according to the calling convention
// stack_etnry is in RSI according to the calling convention

            "mov %rsi, %rsp\n"

            "xor %rax, %rax\n"
            "xor %rbx, %rbx\n"
            "xor %rcx, %rcx\n"
            "xor %rdx, %rdx\n"

            "xor %r9, %r9\n"
            "xor %r8, %r8\n"
            "xor %r10, %r10\n"
            "xor %r11, %r11\n"
            "xor %r12, %r12\n"
            "xor %r13, %r13\n"
            "xor %r14, %r14\n"
            "xor %r15, %r15\n"

            "xor %rsi, %rsi\n"
            "xor %rbp, %rbp\n"

            "jmp *%rdi"
            );
}

void linux_x64_loader::load_and_run() const {
    opened_unix_file file = open_file(file_id);
    switch (check_format(file)) {
        case executable_file_format::ELF: {
            opened_elf<64> elf64 = unix_based_elf_loader<64>::cast_to_elf(file);
            if (validate_elf(elf64)) {
                unix_based_elf_loader<64>::load_and_run_elf(elf64,
                                                            unix_based_elf_loader<64>::load_flags.explicit_use_interp);
                break;
            }

            throw "not valid elf for x86_64";

            break;
        }
        case executable_file_format::PE:
            break;
    }
}


bool linux_x64_loader::validate_elf(const opened_elf<64> &elf) const {
    // check matching elf class
    if (elf.get_header()->e_ident[EI_CLASS] != ELFCLASS64)
        return false;

    // check matching OS
    if (!validate_elf_os<64>(elf.get_header(), ELFOSABI_LINUX) &&
        !validate_elf_os<64>(elf.get_header(), ELFOSABI_SYSV) // Linux support Unix System V
            )
        return false;

    // check matching ISAs
    if (!validate_elf_architecture<64>(elf.get_header(), EM_X86_64))
        return false;

    // check type of executable ELF
    if (!validate_elf_type<64>(elf.get_header(), ET_EXEC) && !validate_elf_type<64>(elf.get_header(), ET_DYN))
        return false;

    // check valid version
    if (!validate_elf_version<64>(elf.get_header())) return false;

    return true;
}

void linux_x64_loader::jump_entry_elf(void *entry_addr, void *stack_addr, const opened_elf<64> &elf) const {
    jump_entry(entry_addr, stack_addr);
}


void
linux_x64_loader::relocate_rela(typename elf_file<64>::addr reloc_offset, size_t reloc_type, size_t relocation_value,
                                ssize_t addend, typename elf_file<64>::sym *sym, size_t the_load_bias) const {
    switch (reloc_type) {
        case R_X86_64_NONE: {
            break;
        }

        case R_X86_64_64: {
            *(elf_file<64>::word *) reloc_offset = ((elf_file<64>::word) relocation_value) + addend;
            break;
        }

        case R_X86_64_COPY: { // copy symbol value
            if (relocation_value == 0) throw "relocation value 0 when COPY";
            if (elf_file<64>::ELF_ST_TYPE(sym->st_info) != STT_OBJECT) throw "not object when COPY";
            if (sym->st_size == 0) throw "size 0 when COPY";

            memcpy(reinterpret_cast<void *>(reloc_offset), (void *) relocation_value, sym->st_size);
            break;
        }

        case R_X86_64_RELATIVE: {
            *(elf_file<64>::word *) reloc_offset = addend + ((elf_file<64>::word) the_load_bias);
            break;
        }
        case R_X86_64_IRELATIVE: {
            *(elf_file<64>::word *) reloc_offset = reinterpret_cast<elf_file<64>::word>(((void *(*)()) (
                    addend + ((elf_file<64>::word) the_load_bias)
            ))());
            break;
        }

        case R_X86_64_GLOB_DAT: {
            *(elf_file<64>::word *) reloc_offset = (elf_file<64>::word) relocation_value;
            break;
        }
        case R_X86_64_JUMP_SLOT: {
            if (relocation_value == 0) {
                throw "undefined relocation";
//                *(elf_file<64>::word *) reloc_offset = reinterpret_cast<elf_file<64>::word>(&undefined);
                break;
            }
            *(elf_file<64>::word *) reloc_offset = (elf_file<64>::word) relocation_value;
            break;
        }

        default: {
            throw "unsupported type of RELA relocation";
        }
    }
}

void
linux_x64_loader::relocate_rel(typename elf_file<64>::addr reloc_offset, size_t reloc_type, size_t relocation_value,
                               typename elf_file<64>::sym *sym, size_t the_load_bias) const {
    // unlikely that 64 bit elf uses REL
    switch (reloc_type) {

        default: {
            throw "unsupported type of REL relocation";
        }
    }
}

void linux_x64_loader::exit(const opened_elf<64> &elf) const {
    std::exit(0);
}

std::vector<std::string> linux_x64_loader::get_possible_search_prefixes() const {
    return {"/lib/x86_64-linux-gnu/", "/lib64/"};
}

