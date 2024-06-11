#ifndef ELF_TOOLCHAIN_ENCRYPTION_H
#define ELF_TOOLCHAIN_ENCRYPTION_H

#include "../utils/udata.h"

// AES_256_CBC
struct encryption {
    udata key;
    bool salted;
};

encryption convert_to_encryption(const udata &src);

udata convert_to_data(const encryption &enc);

udata perform_encrypt_decrypt(const udata &content, const encryption &enc, bool encrypt);

#endif //ELF_TOOLCHAIN_ENCRYPTION_H