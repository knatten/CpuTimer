#include "CpuTimer.h"

#include <benchmark/benchmark.h>

namespace
{
    void BM_RepeatedSingleTimerElapsedCalls(benchmark::State &state)
    {
        knatten::CpuTimer::RealTimer timer;
        timer.start();
        for (auto _ : state)
        {
            benchmark::DoNotOptimize(timer.elapsed());
        }
    }

    void BM_RepeatedSingleTimerCreationAndElapsedCalls(benchmark::State &state)
    {
        for (auto _ : state)
        {
            knatten::CpuTimer::RealTimer timer;
            timer.start();
            benchmark::DoNotOptimize(timer.elapsed());
        }
    }
} // namespace

BENCHMARK(BM_RepeatedSingleTimerElapsedCalls);
BENCHMARK(BM_RepeatedSingleTimerCreationAndElapsedCalls);

BENCHMARK_MAIN();