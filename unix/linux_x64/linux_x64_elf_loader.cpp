#include "linux_x64_elf_loader.h"

bool linux_x64_elf_loader::validate_elf(const elf_file &elf) const {
    // check matching elf class
    if (elf.get_header()->e_ident[EI_CLASS] != ELFCLASS64) {
        cerr << "bogus class " << hex << elf.get_header()->e_ident[EI_CLASS] << endl;
        return false;
    }

    // check matching OS
    if (elf.get_os_type() != ELFOSABI_LINUX &&
        elf.get_os_type() != ELFOSABI_SYSV) { // Linux support Unix System V
        cerr << "bogus os abi" << endl;
        return false;
    }

    // check matching ISAs
    if (elf.get_architecture_type() != EM_X86_64) {
        cerr << "bogus architecture" << endl;
        return false;
    }

    // check type of executable ELF
    if (elf.get_object_type() != ET_EXEC && elf.get_object_type() != ET_DYN) {
        cerr << "bogus type" << endl;
        return false;
    }

    // check valid version
    if (elf.get_version() != EV_CURRENT) {
        cerr << "bogus version" << endl;
        return false;
    }

    return true;
}

void linux_x64_elf_loader::relocate_rela(elf_file::addr reloc_offset, size_t reloc_type, size_t relocation_value,
                                         ssize_t addend, elf_file::sym *sym, size_t the_load_bias) const {
    switch (reloc_type) {
        case R_X86_64_NONE: {
            break;
        }

        case R_X86_64_64: {
            *(elf_file::word *) reloc_offset = ((elf_file::word) relocation_value) + addend;
            break;
        }

        case R_X86_64_COPY: { // copy symbol value
            if (relocation_value == 0) throw "relocation value 0 when COPY";
            if (elf_file::ELF_ST_TYPE(sym->st_info) != STT_OBJECT) throw "not object when COPY";
            if (sym->st_size == 0) throw "size 0 when COPY";

            memcpy(reinterpret_cast<void *>(reloc_offset), (void *) relocation_value, sym->st_size);
            break;
        }

        case R_X86_64_RELATIVE: {
            *(elf_file::word *) reloc_offset = addend + ((elf_file::word) the_load_bias);
            break;
        }
        case R_X86_64_IRELATIVE: {
            *(elf_file::word *) reloc_offset = reinterpret_cast<elf_file::word>(((void *(*)()) (
                    addend + ((elf_file::word) the_load_bias)
            ))());
            break;
        }

        case R_X86_64_GLOB_DAT: {
            *(elf_file::word *) reloc_offset = (elf_file::word) relocation_value;
            break;
        }
        case R_X86_64_JUMP_SLOT: {
            if (relocation_value == 0) {
                throw "undefined relocation";
//                *(elf_file::word *) reloc_offset = reinterpret_cast<elf_file::word>(&undefined);
                break;
            }
            *(elf_file::word *) reloc_offset = (elf_file::word) relocation_value;
            break;
        }

        default: {
            throw "unsupported type of RELA relocation";
        }
    }
}

void linux_x64_elf_loader::relocate_rel(elf_file::addr reloc_offset, size_t reloc_type, size_t relocation_value,
                                        elf_file::sym *sym, size_t the_load_bias) const {
    // unlikely that 64 bit elf uses REL
    switch (reloc_type) {

        default: {
            throw "unsupported type of REL relocation";
        }
    }
}

void jump_entry(volatile void *exec_entry, volatile void *stack_entry) {
    asm(
// exec_entry is in RDI according to the calling convention
// stack_entry is in RSI according to the calling convention

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

void linux_x64_elf_loader::jump_entry_elf(void *entry_addr, void *stack_addr,
                                          const basic_unix_elf_loader::elf_file &file) const {
    jump_entry(entry_addr, stack_addr);
}

vector<string> linux_x64_elf_loader::get_possible_search_prefixes() const {
    return {"/lib/x86_64-linux-gnu/", "/lib64/"};
}
