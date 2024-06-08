#include "jump_signatures.h"

unix_elf_jump_entry_signature(linux_x64, jmp) {
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

unix_elf_jump_entry_signature(linux_x64, ret) {
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

            "push %rdi\n"
            "ret"

            );
}