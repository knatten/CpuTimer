project(Benchmark)

find_package(benchmark REQUIRED CONFIG)
add_executable(benchmark_timer benchmark.cpp)
target_link_libraries(benchmark_timer PRIVATE benchmark::benchmark CpuTimer)
