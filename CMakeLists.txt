cmake_minimum_required(VERSION 3.26)

project(Loader LANGUAGES C CXX ASM)
set(CMAKE_CXX_STANDARD 20)


## link with libboost
#find_package(Boost REQUIRED COMPONENTS program_options)
#include_directories(${Boost_INCLUDE_DIRS})
#link_libraries(${Boost_LIBRARIES})

# link with libdl
link_libraries(${CMAKE_DL_LIBS})

set(ABSTRACT_SRC
        basic/utils/file.h
        basic/loader/basic_loader.h

        basic/exec_file_formats/exec_file_format.h
        basic/exec_file_formats/check_file_format.h

        basic/utils/macros.h
        basic/utils/process_vars.h
        basic/utils/stack.h
        basic/utils/udata.h
)

add_executable(elf_loader_linux_x64 # for regular x64 executables
        ${ABSTRACT_SRC}

        unix/opened_unix_file.cpp
        unix/linux_x64/linux_x64_elf_loader.cpp

        Linux_x64_loader.cpp
)
set_target_properties(elf_loader_linux_x64 PROPERTIES COMPILE_FLAGS "-Wall -pie -fPIC")
