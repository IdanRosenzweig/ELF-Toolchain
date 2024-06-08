#!/bin/bash

# build target directory and Cmake root directory
BUILD_DIR=./build
SOURCE_DIR=./

# build the loader
cmake -S $SOURCE_DIR -B $SOURCE_DIR/cmake-build-debug
cmake --build $SOURCE_DIR/cmake-build-debug -t obfuscator

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
mv -f $SOURCE_DIR/cmake-build-debug/obfuscator $BUILD_DIR/
