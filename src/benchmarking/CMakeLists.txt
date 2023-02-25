project(Benchmark)

find_package(benchmark 1.7.1 REQUIRED CONFIG)
add_executable(benchmark_timer benchmark.cpp)
target_link_libraries(benchmark_timer PRIVATE benchmark::benchmark CpuTimer)
