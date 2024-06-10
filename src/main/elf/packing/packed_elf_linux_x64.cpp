#include "../../../exec_file_formats/elf/packing/packed_elf.h"

#include "../../../linux_x64/elf/linux_x64_elf_loader.h"
#include "../../../linux_x64/elf/jump_signatures.h"

#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char *argv[], char *env[]) {
    try {
        // prepare the loader
        linux_x64_elf_loader loader;

        loader.basic_unix_elf_loader::setProcVar({argc, argv, env});
        loader.basic_elf_loader::setLoadFlags({false});

        loader.basic_unix_elf_loader::setJumpSignature(
                _linux_x64_elf_jump_entry_signature_ret); // change to somewhat obfuscated jump signate (the OEP)

        // call the packed file with our loader
        packed_elf packedElf(&loader);
        packedElf.run_packed_data();

    } catch (const char *msg) {
        std::cout << "Exception: " << msg << "\n";
    } catch (...) {
        std::cout << "unknown exception\n";
    }
}
