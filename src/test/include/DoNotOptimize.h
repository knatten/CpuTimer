#pragma once

// This is a simplified version of Google Benchmark's DoNotOptimize.
// We don't bother with the full version since we don't care about overhead.
// https://github.com/google/benchmark/blob/main/include/benchmark/benchmark.h
template <class Tp> inline void DoNotOptimize(const Tp &value)
{
    asm volatile("" : : "r,m"(value) : "memory");
}

template <class Tp> inline void DoNotOptimize(Tp &value)
{
#if defined(__clang__)
    asm volatile("" : "+r,m"(value) : : "memory");
#else
    asm volatile("" : "+m,r"(value) : : "memory");
#endif
}
