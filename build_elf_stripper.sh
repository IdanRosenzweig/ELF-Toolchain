#!/bin/bash

STRIP_TARGET_NAME='elf_stripper'

# build target directory and Cmake root directory
TARGET_BUILD_DIR=./build
CMAKE_BUILD_DIR=./cmake-build


# build the loader
cmake -DCMAKE_BUILD_TYPE=Release -S ./ -B $CMAKE_BUILD_DIR
cmake --build $CMAKE_BUILD_DIR -t $STRIP_TARGET_NAME

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
mv -f $CMAKE_BUILD_DIR/$STRIP_TARGET_NAME $TARGET_BUILD_DIR/
