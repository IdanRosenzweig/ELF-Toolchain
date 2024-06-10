#include "check_file_format.h"

#include "elf/basic_elf_file.h"

executable_file_format find_executable_format(raw_file &file) {
    if (check_elf_magic(file))
        return executable_file_format::ELF;
//    else if () // .. pe
//    else if () // .. macho

    throw "no standard executable file format found";
}