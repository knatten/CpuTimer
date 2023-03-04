#!/bin/bash

if [ $UID != 0 ]; then
  SUDO=sudo
else
  SUDO=""
fi

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

$SUDO apt-get update || exit $?
$SUDO apt-get install --assume-yes cmake ninja-build clang clang-tidy || exit $?
pip install -r $SCRIPT_DIR/requirements.txt || exit $?
conan profile detect || exit $?