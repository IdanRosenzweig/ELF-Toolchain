import os
import sys

def convert(file_path):

    # generate encryption data/key
    encryption_key_file = "encryption_data"
    with open(encryption_key_file, "wb") as f:
        f.write(open("/dev/random", "rb").read(50))

    # actually encrypt file
    encrypted_file_path = "./encrypted_file"
    os.system("openssl enc -aes-256-cbc -in " + file_path + " -out " + encrypted_file_path + " -pbkdf2 -pass file:" + encryption_key_file)

    array = ""

    # includes and so
    array += "#include <stdint.h>\n"
    array += "#include <cstdlib>\n\n"

    array += "using namespace std;\n\n"

    # file content
    with open(encrypted_file_path, "rb") as f:
        data = f.read()

    array += "uint8_t raw_data[] = {"
    for byte in data:
        array += hex(byte) + ", "
    array += "};\n"
    array += "uint8_t* packed_data = raw_data;\n\n"
    array += "size_t packed_data_sz = " + str(len(data)) + ";\n\n"

    # file path
    array += "uint8_t* file_path = (uint8_t*) \"" + file_path + "\";\n"
    array += "size_t file_path_len = " + str(len(file_path)) + ";\n\n"

    # encryption related
    with open(encryption_key_file, "rb") as f:
        enc = f.read()

    array += "uint8_t raw_encryption[] = {"
    for byte in enc:
        array += hex(byte) + ", "
    array += "};\n"
    array += "uint8_t* encryption_data = raw_encryption;\n\n"
    array += "size_t encryption_data_sz = " + str(len(enc)) + ";\n\n"

    return array


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python bin_file_to_header.py <file_path>")
        sys.exit(1)

    input_file = sys.argv[1]

    content = convert(input_file)

    with open(f"packed_data.cpp", 'w') as file:
        file.write(content)

    print(f"file 'packed_data.h' has been created")
