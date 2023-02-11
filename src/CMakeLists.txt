cmake_minimum_required(VERSION 3.22)
project(CpuClock)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined -Wall")

enable_testing()

add_library(CpuClock INTERFACE)
target_include_directories(CpuClock INTERFACE include)

add_subdirectory(test)