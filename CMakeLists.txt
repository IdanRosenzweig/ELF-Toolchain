cmake_minimum_required(VERSION 3.26)

project(Loader
        LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 20)

add_executable(Linux_x64_loader # for regular x64 executables
        Linux_x64_loader.cpp

        basic/basic_loader.h
        basic/basic_elf_loader.h
        basic/file_id_entity.h
        basic/flags_entity.h

        exec_file_formats/exec_file_format.h

        exec_file_formats/elf/elf_file.h
        exec_file_formats/elf/elf_lib.h
        exec_file_formats/elf/elf_flags.h

        utils/macros.h
        utils/stack.h
        utils/proc_var.h

        unix_based/unix_based_loader.h
        unix_based/unix_based_loader.cpp
        unix_based/unix_based_elf_loader.h
        unix_based/opened_unix_file.cpp
        unix_based/opened_unix_file.h
        unix_based/opened_elf_file.h

        unix_based/linux_x64/linux_x64_loader.h
        unix_based/linux_x64/linux_x64_loader.cpp

)
target_link_libraries(Linux_x64_loader ${CMAKE_DL_LIBS})
set_target_properties(Linux_x64_loader PROPERTIES COMPILE_FLAGS "-Wall -pie -fPIC")
