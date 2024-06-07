#ifndef LOADER_OBFUSCATION_H
#define LOADER_OBFUSCATION_H

#include "../../utils/udata.h"
#include "encryption.h"

#include <vector>
using namespace std;

enum obfuscation_type {
    ENCRYPTION,
    COMPRESSION,
    ENCODING
};

struct obfuscation {
    obfuscation_type type;
    udata data;
};

udata perform(udata& content, const obfuscation obf) {
    switch (obf.type) {
        case ENCRYPTION: {
            content = perform(obf.data)
        }
    }
}

udata perform(const udata& content, vector<obfuscation> list) {
    udata res = content;
    for (auto& obf : list)
        res = perform(res, obf);

    return res;
}

#endif //LOADER_OBFUSCATION_H
