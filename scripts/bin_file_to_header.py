import os
import sys

def convert(input_file, key_file, orig_path):

    array = ""

    # includes and so
    array += "#include <stdint.h>\n"
    array += "#include <cstdlib>\n\n"

    array += "using namespace std;\n\n"

    # file content
    with open(input_file, "rb") as f:
        data = f.read()

    array += "uint8_t raw_data[] = {"
    for byte in data:
        array += hex(byte) + ", "
    array += "};\n"
    array += "uint8_t* packed_data = raw_data;\n\n"
    array += "size_t packed_data_sz = " + str(len(data)) + ";\n\n"

    # obfuscation key content
    with open(key_file, "rb") as f:
        enc = f.read()

    array += "uint8_t raw_obf[] = {"
    for byte in enc:
        array += hex(byte) + ", "
    array += "};\n"
    array += "uint8_t* obf_data = raw_obf;\n\n"
    array += "size_t obf_data_sz = " + str(len(enc)) + ";\n\n"

    # the original file path
    array += "uint8_t* file_path = (uint8_t*) \"" + orig_path + "\";\n"
    array += "size_t file_path_len = " + str(len(orig_path)) + ";\n\n"

    return array


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python bin_file_to_header.py <obfuscated data file path> <obfuscation key file path> <original path>")
        sys.exit(1)

    input_file = sys.argv[1]
    key_file = sys.argv[2]
    orig_path = sys.argv[3]

    content = convert(input_file, key_file, orig_path)

    with open(f"packed_data.cpp", 'w') as file:
        file.write(content)

    print(f"file 'packed_data.h' has been created")
