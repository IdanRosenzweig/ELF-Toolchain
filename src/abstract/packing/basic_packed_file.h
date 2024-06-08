#ifndef LOADER_BASIC_PACKED_FILE_H
#define LOADER_BASIC_PACKED_FILE_H

#include "../utils/udata.h"

#include "../data_obfuscation/obfuscation.h"

#include <memory>
#include <cstring>

using namespace std;

/** values given at compile time */
// variables for the packed data
extern uint8_t *packed_data;
extern size_t packed_data_sz;

// obfuscation data that was done on the packed data
extern uint8_t *obf_data;
extern size_t obf_data_sz;

// the original path of the packed executable
extern uint8_t *file_path;
extern size_t file_path_len;


/** class that will be run at execution time */
class basic_packed_file {
public:
    virtual ~basic_packed_file() {}

    void run_packed_data() {
        // the raw data that is packed for us
        udata encrypted_data(packed_data_sz, 0);
        memcpy(encrypted_data.data(), packed_data, packed_data_sz);

        // the obfuscation key
        udata encryption_key(obf_data_sz, 0);
        memcpy(encryption_key.data(), obf_data, obf_data_sz);

        // deobfuscating to the raw packed data
        vector <obfuscation> list;
        convert_to_obfuscations(&list, encryption_key.data());
        udata deobfuscated_data = perform_obfuscations(encrypted_data, list, true);

        string orig_path = string(file_path_len, 0);
        memcpy(orig_path.data(), file_path, file_path_len);

        handle_data(deobfuscated_data, orig_path);
    }

    virtual void handle_data(const udata &data, const string &original_path) = 0;

};

#endif //LOADER_BASIC_PACKED_FILE_H
