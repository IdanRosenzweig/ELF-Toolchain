#ifndef ELF_TOOLCHAIN_ENCODING_H
#define ELF_TOOLCHAIN_ENCODING_H

#include "../utils/udata.h"

// base64
struct encoding {
};

encoding convert_to_encoding(const udata& src);

udata convert_to_data(const encoding &code);

udata perform_encoding_decoding(const udata &content, const encoding &code, bool encode);

#endif //ELF_TOOLCHAIN_ENCODING_H
