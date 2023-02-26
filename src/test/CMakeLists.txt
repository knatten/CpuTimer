find_package(Catch2 3.3.1 REQUIRED CONFIG)
find_package(fmt 9.1.0 REQUIRED CONFIG)

function(add_cpu_timer_test)
    cmake_parse_arguments(ARG "" "NAME" "SOURCES" ${ARGN})
    add_executable(${ARG_NAME} ${ARG_SOURCES})
    target_link_libraries(${ARG_NAME} CpuTimer Catch2::Catch2WithMain fmt::fmt)
    set_target_properties(${ARG_NAME} PROPERTIES CXX_STANDARD 17)
    target_include_directories(${ARG_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME ${ARG_NAME} COMMAND ${ARG_NAME})
endfunction()

add_cpu_timer_test(NAME TestCpuTimer SOURCES test.cpp)