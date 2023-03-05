#include "CpuTimer.h"

#include <chrono>
#include <iostream>
#include <thread>

void doSomeWork()
{
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
int main()
{
    // RealTimer measures the real/wall time. Other alternatives are:
    // - ProcessTimer for the CPU time (user or kernel mode) of this process
    // - ThreadTimer for the CPU time (user or kernel mode) of this thread

    // Initialising the timer does not start it
    knatten::CpuTimer::RealTimer realTimer;

    // Start the timer
    realTimer.start();

    // Do some work
    doSomeWork();

    // Get the elapsed time, in nanoseconds by default
    std::cout << "Doing some work took " << realTimer.elapsed().count()
              << " nanoseconds\n\n";

    // Call .start() again to restart the timer
    realTimer.start();

    // Get the elapsed time, in your chosen unit
    std::cout << "Not doing anything after restart took "
              << realTimer.elapsed<std::chrono::milliseconds>().count()
              << " milliseconds\n\n";

    // Timer measures both real/wall time, process time and thread time
    // (it works as RealTimer, ProcessTimer and ThreadTimer all at once)
    knatten::CpuTimer::Timer timer;

    // Start the timer
    timer.start();

    // Do some work
    doSomeWork();

    // Get the elapsed real/wall, process and thread time
    // (Notice that process and thread should be ~0 since we're just sleeping
    const auto elapsed = timer.elapsed<std::chrono::milliseconds>();
    std::cout << "Doing some work took:\n  " << elapsed.realTime.count()
              << " milliseconds of real/wall time\n  "
              << elapsed.processTime.count()
              << " milliseconds of process CPU time\n  "
              << elapsed.threadTime.count()
              << " milliseconds of thread CPU time\n";
}