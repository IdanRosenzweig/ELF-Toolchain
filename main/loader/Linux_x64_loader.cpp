#include "../../unix/linux_x64/linux_x64_elf_loader.h"

#include <iostream>
#include <boost/program_options.hpp>
using namespace std;

int main(int argc, char *argv[], char *env[]) {
//    namespace po = boost::program_options;
//
//    po::options_description opts("load ELF executables into a Linux x64 OS");
//    opts.add_options()
//            ("help,h", "print tool use description")
//            ("file,f", po::value<string>(), "path to the ELF file to load")
//            ("args,a", po::value<vector<string>>()->multitoken(), "command line arguments to pass to the file")
//;
//
//    po::variables_map vm;
//    po::store(po::parse_command_line(argc, argv, opts), vm);
//    po::notify(vm);
//
//    if (vm.count("help")) {
//        std::cout << opts << endl;
//        return 1;
//    }
//
//    // file to load
//    if (!vm.count("file")) {
//        std::cout << opts << endl;
//        return 1;
//    }
//    string file_path = vm["file"].as<string>();
//
//    // process arguments
//    process_vars vars;
//    if (vm.count("args")) {
//        vars.args = vm["args"].as<vector<string>>();
//        vars.argc = vars.args.size();
//    }
//    int i = 0;
//    while (env[i] != nullptr) {
//        vars.env.push_back(string(env[i]));
//        i++;
//    }

    // loading
    linux_x64_elf_loader loader;

    loader.basic_unix_elf_loader::setProcVar({argc - 1, argv + 1, env});
    loader.basic_elf_loader::setLoadFlags({false});

    try {
        loader.load_and_run_file(make_unique<opened_unix_file>(argv[1]));
    } catch (const char *msg) {
        std::cout << "Exception: " << msg << "\n";
    } catch (...) {
        std::cout << "unknown exception\n";
    }

}