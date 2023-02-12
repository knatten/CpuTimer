cmake_minimum_required(VERSION 3.22)
project(CpuTimer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined -Wall")

enable_testing()

add_library(CpuTimer INTERFACE)
target_include_directories(CpuTimer INTERFACE include)

add_subdirectory(test)
