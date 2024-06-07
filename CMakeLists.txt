cmake_minimum_required(VERSION 3.26)

project(Loader LANGUAGES C CXX ASM)
set(CMAKE_CXX_STANDARD 20)


# find python interpreter
find_package(Python3 REQUIRED)
set(PYTHON_PATH /usr/bin/python3)

# link with libboost
find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

# link with libdl
link_libraries(${CMAKE_DL_LIBS})


set(ABSTRACT_SRC
        basic/utils/basic_file.h
        basic/loader/basic_loader.h

        basic/exec_file_formats/exec_file_format.h
        basic/exec_file_formats/check_file_format.h
        basic/exec_file_formats/elf/elf.h

        basic/utils/macros.h
        basic/utils/process_vars.h
        basic/utils/stack.h
        basic/utils/udata.h

        basic/packer/basic_packer.h
        basic/packer/obfuscation/obfuscation.h
        basic/packer/obfuscation/encryption.h
)


add_executable(elf_loader_linux_x64 # for regular x64 executables
        ${ABSTRACT_SRC}

        unix/opened_unix_file.cpp
        unix/file_helper_funcs.cpp
        unix/linux_x64/linux_x64_elf_loader.cpp

        main/loader/Linux_x64_loader.cpp
)


add_executable(obfuscator
        ${ABSTRACT_SRC}

        unix/opened_unix_file.cpp
        unix/linux_x64/linux_x64_elf_loader.cpp

        main/packer/obfuscator.cpp
)

add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/packed_data.cpp
        COMMAND ${CMAKE_COMMAND} -E echo "generating packed_data.cpp"
        COMMAND ${PYTHON_PATH} ${CMAKE_SOURCE_DIR}/scripts/bin_file_to_header.py ${PACKED_FILE_PATH}
        DEPENDS ${PACKED_FILE_PATH}
)
add_custom_target(generate_packed_file_header DEPENDS ${CMAKE_BINARY_DIR}/packed_data.cpp)

add_executable(packed_file
        ${ABSTRACT_SRC}

        unix/opened_unix_file.cpp
        unix/file_helper_funcs.cpp
        unix/file_helper_funcs.h
        unix/linux_x64/linux_x64_elf_loader.cpp

        main/packer/packed_file.cpp

        ${CMAKE_BINARY_DIR}/packed_data.cpp
)
add_dependencies(packed_file generate_packed_file_header)
