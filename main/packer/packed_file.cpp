#include "../../basic/packer/elf/elf_packer.h"
#include "../../basic/packer/obfuscation/obfuscation.h"

#include "../../unix/linux_x64/linux_x64_elf_loader.h"
#include "../../unix/file_helper_funcs.h"

#include <iostream>
#include <cstring>
#include <boost/program_options.hpp>

using namespace std;

// variables for the packed data
extern uint8_t *packed_data;
extern size_t packed_data_sz;

extern uint8_t *file_path;
extern size_t file_path_len;

extern uint8_t *encryption_data;
extern size_t encryption_data_sz;


int main(int argc, char *argv[], char *env[]) {
    try {

        // the raw data that is packed for us
        udata encrypted_data(packed_data_sz, 0);
        memcpy(encrypted_data.data(), packed_data, packed_data_sz);

        // the encryption key
        udata encryption_key(encryption_data_sz, 0);
        memcpy(encryption_key.data(), encryption_data, encryption_data_sz);

        // decrypting to get the actual data of the original file
        vector<obfuscation> list;
        convert_to_obfuscations(&list, encryption_key.data());
        udata file_data_decrypted = perform_obfuscations(encrypted_data, list, true);

        // wrapping the file's data as opened_unix_file
        auto file = make_unique<opened_unix_file>();
        file->file_data = file_data_decrypted;
        file->path = string(file_path_len, 0);
        memcpy(file->path.data(), file_path, file_path_len);

        // just run it
        linux_x64_elf_loader loader;

        loader.basic_unix_elf_loader::setProcVar({argc, argv, env});
        loader.basic_elf_loader::setLoadFlags({true});

        loader.load_and_run_file(std::move(file));

    } catch (const char *msg) {
        std::cout << "Exception: " << msg << "\n";
    } catch (...) {
        std::cout << "unknown exception\n";
    }

}
