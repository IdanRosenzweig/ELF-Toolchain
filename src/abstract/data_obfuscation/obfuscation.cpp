#include "obfuscation.h"

#include "encryption.h"

int convert_to_obfuscation(obfuscation *obf, uint8_t *src) {
    const uint8_t *buff = src;

    obf->type = *(obfuscation_type*) buff;
    buff += sizeof(obfuscation_type);

    buff += decode_data(&obf->data, buff);

    return buff - src;
}

udata convert_to_data(const obfuscation &obf) {
#define BUFF_LEN 10000
    uint8_t buff[BUFF_LEN];
    uint8_t *curr = buff;

    *(obfuscation_type*)curr = obf.type;
    curr += sizeof(obfuscation_type);

    curr += encode_data(obf.data, curr);

    return udata(buff, curr - buff);
}


int convert_to_obfuscations(obfuscation_list *list, uint8_t *src) {
    uint8_t * buff = src;

    while (true) {
        obfuscation next_obf;
        buff += convert_to_obfuscation(&next_obf, buff);

        if (next_obf.type == NONE) break;
        else list->push_back(next_obf);
    }

    return buff - src;
}

udata convert_to_data(const obfuscation_list &list) {
    udata res;

    for (auto& obf : list)
        res += convert_to_data(obf);
    res += convert_to_data(obfuscation{NONE, {(uint8_t*) "none"}});

    return res;
}


udata perform_obfuscation(const udata &content, const obfuscation &obf, bool reverse) {
    udata res = content;

    switch (obf.type) {
        case ENCRYPTION: {
            res = perform_encrypt_decrypt(res, convert_to(obf.data), !reverse);
            break;
        }
        case COMPRESSION: {
            break;
        }
        case ENCODING: {
            break;
        }
    }

    return res;
}

udata perform_obfuscations(const udata &content, const obfuscation_list &list, bool reverse) {
    udata res = content;
    for (auto &obf: list)
        res = perform_obfuscation(res, obf, reverse);

    return res;
}
