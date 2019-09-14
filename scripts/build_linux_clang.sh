#!/bin/bash

export CC=clang
export CXX=clang++

echo "${BASH_SOURCE[0]}"
SOURCE=$(readlink -f "${BASH_SOURCE[0]}")
SOURCE_DIR=$(dirname $SOURCE)/..
BUILD_ROOT="$SOURCE_DIR/.build"

echo $SOURCE_DIR

pushd .

cd ..
mkdir -p $BUILD_ROOT/_conan_linux_clang
cd $BUILD_ROOT/_conan_linux_clang

conan install $SOURCE_DIR -p clang

cd ..
mkdir $BUILD_ROOT/_build_linux_clang
cd $BUILD_ROOT/_build_linux_clang
cmake $SOURCE_DIR -G "Unix Makefiles" -DCONAN_BUILD_DIR=.build/_conan_linux_clang


make -j 
popd 
