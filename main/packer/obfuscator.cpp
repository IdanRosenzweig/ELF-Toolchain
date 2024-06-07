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

    po::options_description opts(
            "obfuscate files with arbitrary amounts of encryption/compression/encoding layers.\n each obfuscated file is associated with obfuscation key file (inspired from encryption key files) which can be used to deobfuscate the file back");
    opts.add_options()
            ("help,h", "print tool use description")
            ("file,f", po::value<string>(), "path to the file to obfuscate")
#define DEFAULT_OUTPUT_NAME "obfuscated_data"
            ("output,o", po::value<string>(),
             "name/path for the output of the obfuscated file. default is\"" DEFAULT_OUTPUT_NAME "\"")
#define DEFAULT_KEY_NAME "obfuscation_key"
            ("key,k", po::value<string>(),
             "name/path for the output of the obfuscation key file. default is \"" DEFAULT_KEY_NAME "\"");

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

    // output file
    string output_file_path;
    if (vm.count("output"))
        output_file_path = vm["output"].as<string>();
    else output_file_path = DEFAULT_OUTPUT_NAME;

    // output key
    string key_path;
    if (vm.count("key"))
        key_path = vm["key"].as<string>();
    else key_path = DEFAULT_KEY_NAME;

    // obfuscate the content
    udata file_content = get_data_from_file(file_path);
    vector<obfuscation> obfuscations{
            {obfuscation_type::ENCRYPTION,
             convert_to_data(encryption{(uint8_t *) "test", false})
            }
    };

    udata obfuscated_data = perform_obfuscations(file_content, obfuscations, false);


    // store obfuscated content in file
    write_data_to_file(obfuscated_data, output_file_path);
    cout << "generated obfuscated file: " << output_file_path << endl;

    // store obfuscation key in file
    udata obfs_key = convert_to_data(obfuscations);
    write_data_to_file(obfs_key, key_path);
    cout << "generated obfuscations key file: " << key_path << endl;

}
