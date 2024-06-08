#!/bin/bash

if [ "$#" -ne 1 ];
then
  echo "provide a path to the executable file"
  exit
fi

echo "path for the executable is $1"

# build target directory and Cmake root directory
TARGET_BUILD_DIR=./build
CMAKE_BUILD_DIR=./cmake-build

# build the obfuscator
cmake -DCMAKE_BUILD_TYPE=Release -S ./ -B $CMAKE_BUILD_DIR
cmake --build $CMAKE_BUILD_DIR -t obfuscator

# obfuscate the executable
$CMAKE_BUILD_DIR/obfuscator -f $1 -o obf_data -k obf_key

# build
cmake -DCMAKE_BUILD_TYPE=Release -S ./ -B $CMAKE_BUILD_DIR -DOBFUSCATED_FILE=obf_data -DOBFUSCATION_KEY=obf_key -DORIG_PATH=$1
cmake --build $CMAKE_BUILD_DIR -t packed_file

# delete any obfuscation data (build has finished)
rm -f obf_data obf_key

# check if build actually failed
if [ $? -ne 0 ]; then
  echo "Build failed"
  rm -rf $CMAKE_BUILD_DIR
  exit
fi

# move the output of the cmake to the custom build directory
if ! (stat $TARGET_BUILD_DIR &> /dev/null); then
  mkdir $TARGET_BUILD_DIR
fi
mv -f $CMAKE_BUILD_DIR/packed_file $TARGET_BUILD_DIR/
