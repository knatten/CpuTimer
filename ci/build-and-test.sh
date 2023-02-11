#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/common.sh || exit $?

if [ $# != 1 ]; then
  echo "USAGE: $0 <Debug|Release>"
  exit 1
fi

CONFIG=$1

BUILD_DIR=$ROOT_DIR/build/ci/$OS/$CONFIG
echo "Building and testing in '$BUILD_DIR' with CMAKE_BUILD_TYPE '$CONFIG'"

mkdir -p $BUILD_DIR || exit $?
cd $BUILD_DIR || exit $?
conan install $SRC_DIR --profile=default -of=. --settings=build_type=$CONFIG --build=missing|| exit $?

cmake -S $SRC_DIR -GNinja -B $BUILD_DIR -DCMAKE_BUILD_TYPE=$CONFIG || exit $?
cmake --build $BUILD_DIR || exit $?

ctest --output-on-failure || exit $?