#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/common.sh

BUILD_DIR=$ROOT_DIR/build/ci/$OS/clang-tidy
echo "Building with clang-tidy in '$BUILD_DIR'"

mkdir -p $BUILD_DIR || exit $?
cd $BUILD_DIR || exit $?
conan install $SRC_DIR --profile=default -of=. --build=missing|| exit $?
cmake -S $SRC_DIR -B $BUILD_DIR -GNinja -DCMAKE_BUILD_TYPE=Release -DCLANG_TIDY=ON || exit $?
cmake --build . || exit $?