#include "../../basic/packer/elf/elf_packer.h"
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

udata decrypt_data(const udata &data, const udata &encryption_file) {
#define PACKED_FILE_NAME "packed_data"
#define ENC_FILE_NAME "enc_file"
#define RES_FILE_NAME "out_file"
    write_data_to_file(data, PACKED_FILE_NAME);
    write_data_to_file(encryption_file, ENC_FILE_NAME);

    system("openssl enc -d -aes-256-cbc -in " PACKED_FILE_NAME " -out " RES_FILE_NAME " -pbkdf2 -pass file:./" ENC_FILE_NAME);
    udata res_data = get_data_from_file(RES_FILE_NAME);

    system("rm -f " PACKED_FILE_NAME " " ENC_FILE_NAME " " RES_FILE_NAME);

    return res_data;
}

int main(int argc, char *argv[], char *env[]) {
    try {

        // the raw data that is packed for us
        udata encrypted_data(packed_data_sz, 0);
        memcpy(encrypted_data.data(), packed_data, packed_data_sz);

        // the encryption file
        udata encryption_file(encryption_data_sz, 0);
        memcpy(encryption_file.data(), encryption_data, encryption_data_sz);

        // decrypting to get the actual data of the original file
        udata file_data_decrypted = decrypt_data(encrypted_data, encryption_file);

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
