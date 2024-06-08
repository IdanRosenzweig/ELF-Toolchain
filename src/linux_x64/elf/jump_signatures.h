#ifndef LOADER_JUMP_SIGNATURES_H
#define LOADER_JUMP_SIGNATURES_H

#include "../../general_unix/elf/basic_unix_elf_loader.h"

unix_elf_jump_entry_signature(linux_x64, jmp);

unix_elf_jump_entry_signature(linux_x64, ret);

#endif //LOADER_JUMP_SIGNATURES_H
