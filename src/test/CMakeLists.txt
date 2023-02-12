set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_BINARY_DIR})
find_package(Catch2 3.3.1 REQUIRED CONFIG)

add_executable(TestCpuTimer test.cpp)
target_link_libraries(TestCpuTimer CpuTimer Catch2::Catch2WithMain)
add_test(NAME TestCpuTimer COMMAND TestCpuTimer)
