# Sophisticated ELF Loader & Packer for Executables and Malware

## overview
I developed a standalone ELF loader for Linux that efficiently parses and loads executable files. The loader supports:
* both statically linked & dynamically linked executables
* PIE and non-PIE code (userspace ASLR)
* program stack initialization
* ELF auxiliary vector initialization
* both independent jump tail invocation and interpreter-dependent invocation 

In addition, I developed a sophisticated ELF packer for executables and malware. The packer can be split into two parts:
* The **data obfuscating** part is responsible for applying various layers of encryption, compression, and encoding over a given data.
All it does essentially is taking a file, obfuscating its content and producing a "key file" which can be used to deobfuscate the data back in the future.
* The **runtime unpacking** part is responsible for deobfuscating the file back, and just loading it to the operating system.

As a whole, the packer will take an executable file, obfuscate it and compile the resulting file and key along with the runtime unpacking software to produce a final executable file which is "packed".

## build
to build the loader, execute `build_loader.sh` at the top directory.
this will create a subdirectory `build` which will contain the loader executable.

to build the obfuscator alove, execute `build_obfuscator.sh` at the top directory.
this will create a subdirectory `build` which will contain the obfuscator executable.

to pack some executable, execute `pack_executable.sh <path to your executable>` at the top directory.
this will create a subdirectory `build` which will contain the packed file.
