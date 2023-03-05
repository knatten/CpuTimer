# CpuTime

C++11 header only timer library (`clock_gettime` wrapper)

![CI](https://github.com/knatten/CpuTimer/actions/workflows/ci.yml/badge.svg)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](code_of_conduct.md)

Allows you to measure

- Real time spent
- CPU time spent
- Thread time spent

Only tested on Linux and macOS

## Example:

You can measure real time with `RealTiemr`, CPU time spent by this process with `ProcessTimer`, and CPU time spent by
this thread with `ThreadTimer`. Here's an example with `RealTimer`, the other two share the same API:

```c++
    // Initialising the timer does not start it
    knatten::CpuTimer::RealTimer realTimer;

    // Start the timer
    realTimer.start();

    // Do some work
    doSomeWork();

    // Get the elapsed time, in nanoseconds by default
    std::cout << "Doing some work took " << realTimer.elapsed().count()
              << " nanoseconds\n\n";
```

You can restart the timer by calling `.start()` again:

```c++

    // Call .start() again to restart the timer
    realTimer.start();
```

You can specify the `chrono` type you want the result in. Above, it defaulted to `std::chrono::nanoseconds`, but you can
set it to other types too:

```c++
realTimer.elapsed<std::chrono::milliseconds>().count()
```

You can also measure all three types at once with the `Timer` class:

```c++
    // Initialising the timer does not start it
    knatten::CpuTimer::Timer timer;

    // Start the timer
    timer.start();

    // Do some work
    doSomeWork();

    const auto elapsed = timer.elapsed<std::chrono::milliseconds>();
    std::cout << "Doing some work took:\n  " << elapsed.realTime.count()
              << " milliseconds of real/wall time\n  "
              << elapsed.processTime.count()
              << " milliseconds of process CPU time\n  "
              << elapsed.threadTime.count()
              << " milliseconds of thread CPU time\n";
```

## Usage

Just copy [src/include/CpuTimer.h](src/include/CpuTimer.h) into your project. See the examples above for usage.

## Building the full project with tests

This is using Conan, if you don't like Conan just have a look at [src/conanfile.py](src/conanfile.py) and install
the dependencies listed in the `requirements` method manually.

- `pip install ci/requirements.txt` (virtualenv recommended)
- `mkdir build && cd build`
- `conan install --build=missing -of=. ../src` (optionally provide a profile with `-pr=<profile>`)
- `cmake ../../src/ -DCMAKE_BUILD_TYPE=<build type>` (`-GNinja` recommended)
- `cmake --build .`
- `ctest` To run the tests

## CI

To run CI locally:

- `./ci/build-and-test.sh <Debug|Relase>` builds the code and runs the tests in Debug/Release
- `./ci/format.sh <--fix|--check>` fixes or checks formatting of all C++ code
- `./ci/clang-tidy.sh` builds the code with Clang Tidy
- `./ci/sanitizer.sh <address|thread|undefined>` builds the code and runs the tests with the chosen sanitizer

## Conan

To create a conan package:

```txt
conan create src
```