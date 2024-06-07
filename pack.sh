#!/bin/bash

if [ "$#" -ne 1 ];
then
  echo "provide a path"
  exit
fi

echo path is $1

# build target directory
BUILD_DIR=./build

# Cmake root directory
SOURCE_DIR=./

# build
cmake -S $SOURCE_DIR -B $SOURCE_DIR/cmake-build-debug -DPACKED_FILE_PATH=$1
cmake --build $SOURCE_DIR/cmake-build-debug -t clean
cmake --build $SOURCE_DIR/cmake-build-debug -t packed_file

if [ $? -ne 0 ]; then
  echo "Build failed"
  rm -rf $BUILD_DIR
  exit
fi

# move to build directory
if ! (stat $BUILD_DIR &> /dev/null); then
  mkdir $BUILD_DIR
fi
mv -f $SOURCE_DIR/cmake-build-debug/packed_file $BUILD_DIR/
