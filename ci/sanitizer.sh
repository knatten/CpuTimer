#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/common.sh || exit $?

if [ $# != 1 ]; then
  echo "USAGE: $0 <address|thread|undefined>"
  exit 1
fi

SANITIZER=$1

BUILD_DIR=$ROOT_DIR/build/ci/$OS/sanitizer/$SANITIZER
echo "Running with sanitizer '$SANITIZER' in '$BUILD_DIR'"

mkdir -p $BUILD_DIR || exit $?
cd $BUILD_DIR || exit $?
conan install $SRC_DIR --profile=default -s build_type=Debug -of=. --build=missing|| exit $?
cmake -S $SRC_DIR -GNinja -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DSANITIZER=$SANITIZER -DTIMER_TOLERANCE=1000|| exit $?
cmake --build $BUILD_DIR || exit $?

ctest --output-on-failure || exit $?