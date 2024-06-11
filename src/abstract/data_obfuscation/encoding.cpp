#include "encoding.h"

#include "../utils/raw_file.h"

encoding convert_to_encoding(const udata& src) {
    encoding code;

    const uint8_t *buff = src.data();

//    code->type = *(encoding_type *) buff;
//    buff += sizeof(encoding_type);

    return code;
}

udata convert_to_data(const encoding &code) {
#define BUFF_LEN 10000
    uint8_t buff[BUFF_LEN];
    uint8_t *curr = buff;

//    *(encoding_type *) curr = code.type;
//    curr += sizeof(encoding_type);

    return udata(buff, curr - buff);
}

udata perform_encoding_decoding(const udata &content, const encoding &code, bool encode) {
#define ORIG_FILE_NAME "orig_data"
#define RES_FILE_NAME "out_file"

    store_to_file_system(ORIG_FILE_NAME, content);

    string command;
    command += "base64";
    command += (encode ? "" : " -d");
    command += " " ORIG_FILE_NAME;
    command += " > " RES_FILE_NAME;

    system(command.c_str());

    udata res_data(open_raw_file(RES_FILE_NAME).content);

    system("rm -f " ORIG_FILE_NAME " " RES_FILE_NAME);

    return res_data;

}