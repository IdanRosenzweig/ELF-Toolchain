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
        src/abstract/utils/macros.h
        src/abstract/utils/process_vars.h
        src/abstract/utils/stack.h
        src/abstract/utils/udata.h
        src/abstract/utils/raw_file.h
        src/abstract/utils/raw_file.cpp

        src/abstract/data_obfuscation/obfuscation.h
        src/abstract/data_obfuscation/obfuscation.cpp
        src/abstract/data_obfuscation/encryption.h
        src/abstract/data_obfuscation/encryption.cpp

        src/abstract/loading/basic_loader.h

        src/abstract/packing/basic_packed_file.h
)

set (EXEC_FORMATS
        src/exec_file_formats/exec_file_format.h
        src/exec_file_formats/check_file_format.h
        src/exec_file_formats/check_file_format.cpp

        src/exec_file_formats/elf/strip/parse.h
        src/exec_file_formats/elf/basic_elf_file.h
)


add_executable(elf_loader_linux_x64 # for regular x64 executables
        ${ABSTRACT_SRC}
        ${EXEC_FORMATS}

        src/linux_x64/elf/linux_x64_elf_loader.cpp
        src/linux_x64/elf/jump_signatures.cpp

        src/main/loading/elf_loader_linux_x64.cpp
)


add_executable(obfuscator
        ${ABSTRACT_SRC}
        ${EXEC_FORMATS}

        src/main/file_obfuscator/obfuscator.cpp
)


add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/packed_data.cpp
        COMMAND ${CMAKE_COMMAND} -E echo "generating packed_data.cpp"
        COMMAND ${PYTHON_PATH} ${CMAKE_SOURCE_DIR}/scripts/bin_file_to_header.py ${CMAKE_SOURCE_DIR}/${OBFUSCATED_FILE} ${CMAKE_SOURCE_DIR}/${OBFUSCATION_KEY} ${ORIG_PATH}
        DEPENDS ${PACKED_FILE_PATH}
)
add_custom_target(generate_packed_file_header DEPENDS ${CMAKE_BINARY_DIR}/packed_data.cpp)

add_executable(packed_file
        ${ABSTRACT_SRC}
        ${EXEC_FORMATS}

        src/linux_x64/elf/linux_x64_elf_loader.cpp
        src/linux_x64/elf/jump_signatures.cpp

        src/main/packing/packed_elf_linux_x64.cpp

        ${CMAKE_BINARY_DIR}/packed_data.cpp
)
add_dependencies(packed_file generate_packed_file_header)
