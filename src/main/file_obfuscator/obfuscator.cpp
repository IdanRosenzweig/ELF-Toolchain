#include "../../abstract/data_obfuscation/obfuscation.h"
#include "../../abstract/data_obfuscation/encryption.h"
#include "../../abstract/utils/raw_file.h"

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

using namespace std;

int main(int argc, char *argv[], char *env[]) {
    namespace po = boost::program_options;

    po::options_description opts(
            "obfuscate files with arbitrary amounts of encryption/compression/encoding layers.\neach obfuscated file is associated with its obfuscation key file which describes the various layers of obsucation that the file has been through.\nthe key file can be used to deobfuscate the file back");
    opts.add_options()
            ("help,h", "print tool use description")
            ("file,f", po::value<string>(), "path to the file to obfuscate")
#define DEFAULT_OUTPUT_FILE_PATH "obfuscated_data"
            ("output,o", po::value<string>(),
             "name/path for the output of the obfuscated content. default is\"" DEFAULT_OUTPUT_FILE_PATH "\"")
#define DEFAULT_KEY_FILE_PATH "obfuscation_key"
            ("key,k", po::value<string>(),
             "name/path for the output of the obfuscation key. default is \"" DEFAULT_KEY_FILE_PATH "\"");

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
    string in_file_path = vm["file"].as<string>();

    // output file
    string out_file_path;
    if (vm.count("output"))
        out_file_path = vm["output"].as<string>();
    else out_file_path = DEFAULT_OUTPUT_FILE_PATH;

    // output key
    string key_file_path;
    if (vm.count("key"))
        key_file_path = vm["key"].as<string>();
    else key_file_path = DEFAULT_KEY_FILE_PATH;


    // retrieve the file's content
    udata file_content = open_raw_file(in_file_path).content;


    // prepare the obfuscations list
    vector <obfuscation> obfuscations{
            {obfuscation_type::ENCRYPTION,
             convert_to_data(encryption{(uint8_t *) "abcdefgabcdefgabcdefgabcdefg", false})
            }
    };

    // obfuscated the data
    udata obfuscated_data = perform_obfuscations(file_content, obfuscations, false);
    // and store the obfuscations list
    udata obfuscation_key = convert_to_data(obfuscations);


    // store obfuscated content in file
    store_to_file_system(out_file_path, obfuscated_data);
    cout << "generated obfuscated file: " << out_file_path << endl;

    // store obfuscation key in file
    store_to_file_system(key_file_path, obfuscation_key);
    cout << "generated obfuscations key file: " << key_file_path << endl;

}
