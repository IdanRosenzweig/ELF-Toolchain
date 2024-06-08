#!/bin/bash

if [ "$#" -ne 1 ];
then
  echo "provide a path to the executable file"
  exit
fi

echo "path for the executable is $1"

# build target directory and Cmake root directory
BUILD_DIR=./build
SOURCE_DIR=./

# build the obfuscator
cmake -S $SOURCE_DIR -B $SOURCE_DIR/cmake-build-debug
cmake --build $SOURCE_DIR/cmake-build-debug -t obfuscator

# obfuscate the executable
$SOURCE_DIR/cmake-build-debug/obfuscator -f $1 -o obf_data -k obf_key

# build
cmake -S $SOURCE_DIR -B $SOURCE_DIR/cmake-build-debug -DOBFUSCATED_FILE=obf_data -DOBFUSCATION_KEY=obf_key -DORIG_PATH=$1
cmake --build $SOURCE_DIR/cmake-build-debug -t packed_file

# delete any obfuscation data (build has finished)
rm -f obf_data obf_key

# check if build actually failed
if [ $? -ne 0 ]; then
  echo "Build failed"
  rm -rf $BUILD_DIR
  exit
fi

# move the output of the cmake to the custom build directory
if ! (stat $BUILD_DIR &> /dev/null); then
  mkdir $BUILD_DIR
fi
mv -f $SOURCE_DIR/cmake-build-debug/packed_file $BUILD_DIR/
