#include "encryption.h"

#include "../utils/raw_file.h"

#include <cstdlib>

using namespace std;

encryption convert_to_encryption(const udata &src) {
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

    store_to_file_system(PACKED_FILE_NAME, content);
    store_to_file_system(ENC_FILE_NAME, enc.key);

    string command;
    command += "openssl enc -aes-256-cbc";
    command += (encrypt ? "" : " -d");
    command += (enc.salted ? " -salt" : "");
    command += " -in ./" PACKED_FILE_NAME;
    command += " -out ./" RES_FILE_NAME;
    command += " -pbkdf2 -pass file:./" ENC_FILE_NAME;

    system(command.c_str());

    udata res_data(open_raw_file(RES_FILE_NAME).content);

    system("rm -f " PACKED_FILE_NAME " " ENC_FILE_NAME " " RES_FILE_NAME);

    return res_data;

}
