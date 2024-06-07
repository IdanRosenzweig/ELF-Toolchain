#ifndef LOADER_ENCRYPTION_H
#define LOADER_ENCRYPTION_H

#include "../../utils/udata.h"

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;

// AES_256_CBC
struct encryption {
    udata key;
    bool salted;

};

encryption convert_to(const udata& src) {
    encryption res;

    const uint8_t * buff = src.data();

    buff += decode_data(&res.key, buff);

    res.salted = *(bool*) buff;
    buff += sizeof(bool);

    return res;
}

udata convert_to_data(const encryption& enc) {
    udata res(sizeof(size_t) + enc.key.size() + sizeof(enc.salted), 0);
    uint8_t * buff = res.data();

    buff += encode_data(enc.key, buff);

    *(bool*) buff = enc.salted;
    buff += sizeof(bool);

    return res;
}

udata perform(const udata& content, const encryption& enc) {
#define PACKED_FILE_NAME "packed_data"
#define ENC_FILE_NAME "enc_file"
#define RES_FILE_NAME "out_file"
    ofstream packed_file(PACKED_FILE_NAME, ios::binary);
    packed_file.write((const char *) content.data(), content.size());
    ofstream enc_key(ENC_FILE_NAME, ios::binary);
    packed_file.write((const char *) enc.key.data(), enc.key.size());

    system("openssl enc -d -aes-256-cbc -in " PACKED_FILE_NAME " -out " RES_FILE_NAME " -pbkdf2 -pass file:./" ENC_FILE_NAME);

    ifstream res_file(RES_FILE_NAME, ios::binary);
    udata res_data((istreambuf_iterator<char>(res_file)),
                   istreambuf_iterator<char>());

    system("rm -f " PACKED_FILE_NAME " " ENC_FILE_NAME " " RES_FILE_NAME);

    return res_data;

}

#endif //LOADER_ENCRYPTION_H