#include "compression.h"

#include "../utils/raw_file.h"

compression convert_to_compression(const udata& src) {
    compression comp;

    const uint8_t *buff = src.data();

//    comp->type = *(compression_type *) buff;
//    buff += sizeof(compression_type);

    return comp;
}

udata convert_to_data(const compression &comp) {
#define BUFF_LEN 10000
    uint8_t buff[BUFF_LEN];
    uint8_t *curr = buff;

//    *(compression_type *) curr = comp.type;
//    curr += sizeof(compression_type);

    return udata(buff, curr - buff);
}

udata perform_compression_decompression(const udata &content, const compression &comp, bool compress) {
#define ORIG_FILE_NAME "orig_data"
#define RES_FILE_NAME "out_file"

    store_to_file_system(ORIG_FILE_NAME, content);

    string command;
    command += (compress ? "gzip" : "gunzip");
    command += " -c " ORIG_FILE_NAME;
    command += " > " RES_FILE_NAME;

    system(command.c_str());

    udata res_data(open_raw_file(RES_FILE_NAME).content);

    system("rm -f " ORIG_FILE_NAME " " RES_FILE_NAME);

    return res_data;

}