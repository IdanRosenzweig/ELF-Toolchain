#ifndef ELF_TOOLCHAIN_OBFUSCATION_H
#define ELF_TOOLCHAIN_OBFUSCATION_H

#include "../utils/udata.h"

#include "encryption.h"
#include "compression.h"
#include "encoding.h"

#include <vector>
using namespace std;


enum obfuscation_type : uint8_t {
    NONE,
    ENCRYPTION,
    COMPRESSION,
    ENCODING
};

struct obfuscation {
    obfuscation_type type;
    udata data;
};

using obfuscation_list = vector<obfuscation>;


int convert_to_obfuscation(obfuscation *obf, uint8_t *src);

udata convert_to_data(const obfuscation &obf);


int convert_to_obfuscations(obfuscation_list *list, uint8_t *src);

udata convert_to_data(const obfuscation_list &list);


udata perform_obfuscation(const udata &content, const obfuscation &obf,
                          bool reverse // either obfuscate or deobfuscate
);

udata perform_obfuscations(const udata &content, const obfuscation_list &list,
                           bool reverse // either obfuscate or deobfuscate
);


#endif //ELF_TOOLCHAIN_OBFUSCATION_H
