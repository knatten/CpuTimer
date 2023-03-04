cmake_minimum_required(VERSION 3.22)
project(CpuTimer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_BINARY_DIR})

# Memory sanitizer requires rebuilding the entire world, don't bother for now
set(VALID_SANITIZERS address thread undefined)
if(SANITIZER)
    if("${SANITIZER}" IN_LIST VALID_SANITIZERS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${SANITIZER}")
    else()
        message(FATAL_ERROR "Unknown SANITIZER option '${SANITIZER}', valid options are ${VALID_SANITIZERS}")
    endif()
endif()

option(CLANG_TIDY "Enable clang-tidy" OFF)
if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif()

enable_testing()

add_library(CpuTimer INTERFACE)
target_include_directories(CpuTimer INTERFACE include)

add_subdirectory(test)
add_subdirectory(benchmarking)
