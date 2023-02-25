# CpuTime

C++11 header only timer library (`clock_gettime` wrapper)

![CI](https://github.com/knatten/CpuTimer/actions/workflows/ci.yml/badge.svg)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](code_of_conduct.md)

Allows you to measure
- Real time spent
- CPU time spent
- Thread time spent

Only tested on Linux and macOS

## Usage

Just copy [src/include/CpuTimer.h](src/include/CpuTimer.h) into your project

## Building the full project with tests

This is using Conan, if you don't like Conan just have a look at [src/conanfile.txt](src/conanfile.txt) and install those dependencies manually.
- `pip install ci/requirements.txt` (virtualenv recommended)
- `mkdir build && cd build`
- `conan install --build=missing -of=. ../src` (optionally provide a profile with `-pr=<profile>`)
- `cmake ../../src/ -DCMAKE_BUILD_TYPE=<build type>` (`-GNinja` recommended)
- `cmake --build .`
- `ctest` To run the tests