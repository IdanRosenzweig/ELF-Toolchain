#ifndef ELF_TOOLCHAIN_COMPRESSION_H
#define ELF_TOOLCHAIN_COMPRESSION_H

#include "../utils/udata.h"

// gzip
struct compression {
};

compression convert_to_compression(const udata& src);

udata convert_to_data(const compression &comp);

udata perform_compression_decompression(const udata &content, const compression &comp, bool compress);

#endif //ELF_TOOLCHAIN_COMPRESSION_H
