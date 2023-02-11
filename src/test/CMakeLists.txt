set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_BINARY_DIR})
find_package(Catch2 3.3.1 REQUIRED CONFIG)

add_executable(TestCpuClock test.cpp)
target_link_libraries(TestCpuClock CpuClock Catch2::Catch2WithMain)
add_test(NAME TestCpuClock COMMAND TestCpuClock)