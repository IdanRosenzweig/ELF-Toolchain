# Toolchain for Loading, Packing and Obfuscating ELF Executables and Malware

## overview
A toolchain I developed for ELF executables and malware, containing:
* standalone loader
* file stripper
* obfuscator for internal structure
* sophisticated packer


The **ELF loader** is a standalone software that efficiently parses and loads ELF executables onto Linux. The loader supports:
* both statically linked & dynamically linked executables
* PIE and non-PIE code (userspace ASLR)
* program stack initialization
* ELF auxiliary vector initialization
* both independent jump tail invocation and interpreter-dependent invocation

The loader is independent of any other tools, and can be used independent for loading ELF files into the operating system.


The **file stripper** would remove any unnecessary sections and segments off ELF executables, leaving a minimalist file which preserves its execution behaviour.
More precisely, the stripper would remove any debugging, notes, or unrelated information stored in the executable, leaving only relevant segments and needed sections.
The file stripper is independent of other software in the toolchain, and can be used as a separate tool.


The **ELF obfuscator** would change the internal structure of ELF executable, without removing any existing data, while preserving the original behaviour of the executable.
More precisely, it would randomize and shuffle segments of the executable, and possible and sections too.
The ELF obfuscator is independent of other software in the toolchain, and can be used as a separate tool.


The **packer** is a sophisticated software that takes ELF executable and generates a new executable which has the same behaviour as the original one, but the original code is encrypted, compressed and encoded.
Therefore, static analysis over the packed executable would produce meaningless results.

The packer software can be split into two parts:
* The **data obfuscator** part is responsible for applying various layers of encryption, compression, and encoding over a given data.
  All it does essentially is taking a file, obfuscating its content and producing a "key file" which can be used to deobfuscate the data back in the future.
* The **runtime unpacking** part is responsible for deobfuscating the file back, and just loading it to the operating system.\
  it uses the ELF loader implemented in the toolchain to load the file (without using any operating system api, in contrast to other packers)

As a whole, the packer would:
1. take ELF executable file
2. obfuscate its content by encrypting, compressing and encoding multiple times (not to confuse with the ELF obfuscator)
3. compile the resulting file and key along with the runtime unpacking software
4. produce a final executable which is "packed"


<br><br/>
(WARNING: stripped/obfuscated executables produces by the toolchain are considered corrupted and invalid. Regular OS loader will not load them. Most probably, the only loader capable of handling them is the one implemented in this toolchain)

## build
to build the ELF loader, execute `build_elf_loader.sh` at the top directory.
this will create a subdirectory `build` which would contain the loader.

to build the ELF stripper, execute `build_elf_stripper.sh` at the top directory.
this will create a subdirectory `build` which would contain the elf stripper.

to build the ELF obfuscator, execute `build_elf_obfuscator.sh` at the top directory.
this will create a subdirectory `build` which would contain the elf obfuscator.

to pack some executable, execute `pack_executable.sh <path_to_the_executable>` at the top directory.
this will generate a packed executable named `packed_file` at the current working directory.

to clean all builds, execute `clean_build.sh`.
