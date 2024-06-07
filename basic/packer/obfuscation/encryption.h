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

encryption convert_to(const udata &src) {
    encryption res;

    const uint8_t *buff = src.data();

    buff += decode_data(&res.key, buff);

    res.salted = *(bool *) buff;
    buff += sizeof(bool);

    return res;
}

udata convert_to_data(const encryption &enc) {

#define BUFF_LEN 10000
    uint8_t buff[BUFF_LEN];
    uint8_t *curr = buff;

    curr += encode_data(enc.key, curr);

    *(bool *) curr = enc.salted;
    curr += sizeof(bool);

    return udata(buff, curr - buff);
}


udata perform_encrypt_decrypt(const udata &content, const encryption &enc, bool encrypt) {
#define PACKED_FILE_NAME "packed_data"
#define ENC_FILE_NAME "enc_file"
#define RES_FILE_NAME "out_file"
    ofstream packed_file(PACKED_FILE_NAME, ios::binary);
    packed_file.write((const char *) content.data(), content.size());
    packed_file.close();
    ofstream enc_key(ENC_FILE_NAME, ios::binary);
    enc_key.write((const char *) enc.key.data(), enc.key.size());
    enc_key.close();

    string command;
    command += "sudo openssl enc -aes-256-cbc";
    command += (encrypt ? "" : " -d");
    command += (enc.salted ? " -salt" : "");
    command += " -in ./" PACKED_FILE_NAME;
    command += " -out ./" RES_FILE_NAME;
    command += " -pbkdf2 -pass file:./" ENC_FILE_NAME;

    system(command.c_str());

    ifstream res_file(RES_FILE_NAME, ios::binary);
    udata res_data((istreambuf_iterator<char>(res_file)),
                   istreambuf_iterator<char>());
    res_file.close();

    system("rm -f " PACKED_FILE_NAME " " ENC_FILE_NAME " " RES_FILE_NAME);

    return res_data;

}

#endif //LOADER_ENCRYPTION_H