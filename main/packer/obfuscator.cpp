#include "../../basic/packer/obfuscation/obfuscation.h"

#include "../../unix/file_helper_funcs.h"

#include <iostream>
#include <cstring>
#include <boost/program_options.hpp>

using namespace std;

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
    namespace po = boost::program_options;

    po::options_description opts("");
    opts.add_options()
            ("help,h", "print tool use description")
            ("file,f", po::value<string>(), "path to the file to obfuscate");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << opts << endl;
        return 1;
    }

    // file to obfuscate
    if (!vm.count("file")) {
        std::cout << opts << endl;
        return 1;
    }
    string file_path = vm["file"].as<string>();

    vector<obfuscation> obfuscations{
            {obfuscation_type::ENCRYPTION,
             encryption_aes_256_cbc::convert_to_data(
                     encryption_aes_256_cbc{udata((uint8_t *) "nvaispurnvlasdcn"), true}
             )}
    };

    udata file_content = get_data_from_file(file_path);

}
