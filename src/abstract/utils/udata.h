#ifndef LOADER_UDATA_H
#define LOADER_UDATA_H

#include <stdint.h>
#include <string>
#include <vector>

typedef std::basic_string <uint8_t> udata;
typedef std::basic_string <uint8_t> ustring;

enum class DATA_ENCODING {
    ESCAPE_BYTE,
#define CODE_ESCAPE_BYTE 0x00
#define CODE_DELIMITER 0x01

};

template<DATA_ENCODING ENCODING = DATA_ENCODING::ESCAPE_BYTE>
int encode_data(const udata &src_data, void *dest_buff) {
    // assuming byte is the minimal unit of memory.

    uint8_t *curr_buff = (uint8_t *) dest_buff;
    int total_bytes = 0;

    const uint8_t *next_byte = src_data.c_str();
    int data_len = src_data.size();
    for (int data_cnt = 0; data_cnt < data_len; data_cnt++) {
        if (*next_byte == CODE_ESCAPE_BYTE) {
            *(curr_buff++) = CODE_ESCAPE_BYTE;
            *(curr_buff++) = *(next_byte++);
            total_bytes += 2;
        } else {
            *(curr_buff++) = *(next_byte++);
            total_bytes += 1;
        }
    }

    *(curr_buff++) = CODE_ESCAPE_BYTE;
    *(curr_buff++) = CODE_DELIMITER;
    total_bytes += 2;

    return total_bytes;
}

template<DATA_ENCODING ENCODING = DATA_ENCODING::ESCAPE_BYTE>
int decode_data(udata *dest_data, const void *src_buff) {
    // assuming byte is the minimal unit of memory.
    // using two-byte delimiter of 0x00,0x01
    // if 0x0 byte appears in the dest_data itself, it would be encoded as 0x00,0x00

    dest_data->clear();
    int total_bytes = 0;

    const uint8_t *next_byte = (const uint8_t *) src_buff;
    while (true) {
        if (*next_byte == CODE_ESCAPE_BYTE) {
            next_byte++;
            total_bytes++;
            if (*next_byte == CODE_DELIMITER) {
                return ++total_bytes;
            } else {
                dest_data->push_back(*(next_byte++));
                total_bytes++;
            }
        } else {
            dest_data->push_back(*(next_byte++));
            total_bytes += 1;
        }
    }
}

#endif //LOADER_UDATA_H
