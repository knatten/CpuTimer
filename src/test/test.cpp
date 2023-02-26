#include "CpuTimer.h"
#include "at_least_matcher.h"

#include <catch2/catch_all.hpp>

#include <chrono>
#include <cmath>
#include <future>
#include <thread>

using Catch::Matchers::Equals;

using namespace knatten::CpuTimer;
using namespace std::chrono_literals;

constexpr int tolerance = 10;

// Only tests the interface of SingleTimer
// The particularities of real/process/thread are tested in the Timer tests
TEST_CASE("Single timer")
{
    RealTimer realTimer;
    realTimer.start();

    // Elapsed time
    std::this_thread::sleep_for(20ms);
    const auto lapTime = realTimer.elapsed<std::chrono::milliseconds>();
    REQUIRE_THAT(lapTime.count(), AtLeast(20, tolerance));

    // Starting again re-starts the timer
    realTimer.start();
    REQUIRE_THAT(realTimer.elapsed<std::chrono::milliseconds>().count(),
                 AtLeast(0, tolerance));
}

TEST_CASE("Single timer checking elapsed time without starting throws")
{
    RealTimer timer;
    REQUIRE_THROWS_WITH(
        timer.elapsed(),
        Equals("Trying to get elapsed time of a timer which was not started"));
}

TEST_CASE("Full timer")
{
    Timer timer;
    timer.start();

    // Elapsed time
    std::this_thread::sleep_for(20ms);
    const auto lapTime = timer.elapsed<std::chrono::milliseconds>();
    REQUIRE_THAT(lapTime.realTime.count(), AtLeast(20, tolerance));
    // Not affected by sleep:
    REQUIRE_THAT(lapTime.processTime.count(), AtLeast(0, tolerance));
    // Not affected by sleep:
    REQUIRE_THAT(lapTime.threadTime.count(), AtLeast(0, tolerance));

    // Starting again re-starts the timer
    timer.start();
    const auto restartedTime = timer.elapsed<std::chrono::milliseconds>();
    REQUIRE_THAT(restartedTime.realTime.count(), AtLeast(0, tolerance));
    // Not affected by sleep:
    REQUIRE_THAT(restartedTime.processTime.count(), AtLeast(0, tolerance));
    // Not affected by sleep:
    REQUIRE_THAT(restartedTime.threadTime.count(), AtLeast(0, tolerance));
}

TEST_CASE("Full timer checking elapsed time without starting throws")
{
    Timer timer;
    REQUIRE_THROWS_WITH(
        timer.elapsed(),
        Equals("Trying to get elapsed time of a timer which was not started"));
}

volatile int doNotOptimize;

void busyWork(std::chrono::nanoseconds duration)
{
    const auto now = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() < now + duration)
    {
        doNotOptimize = sin(doNotOptimize);
    }
}

TEST_CASE("Full timer with busywork on main thread")
{
    Timer timer;
    timer.start();
    busyWork(20ms);
    const auto elapsed = timer.elapsed<std::chrono::milliseconds>();
    REQUIRE_THAT(elapsed.realTime.count(), AtLeast(19, tolerance));
    REQUIRE_THAT(elapsed.processTime.count(), AtLeast(19, tolerance));
    REQUIRE_THAT(elapsed.threadTime.count(), AtLeast(19, tolerance));
}

TEST_CASE("Full timer with busywork on separate thread")
{
    Timer timer;
    timer.start();
    auto future = std::async(std::launch::async, []() { busyWork(20ms); });
    future.wait();
    const auto elapsed = timer.elapsed<std::chrono::milliseconds>();
    REQUIRE_THAT(elapsed.realTime.count(), AtLeast(19, tolerance));
    REQUIRE_THAT(elapsed.processTime.count(), AtLeast(10, tolerance));
    // Work happened on different thread:
    REQUIRE_THAT(elapsed.threadTime.count(), AtLeast(0, tolerance));
}

TEST_CASE("Full timer with busywork on multiple threads")
{
    Timer timer;
    timer.start();
    auto future1 = std::async(std::launch::async, []() { busyWork(30ms); });
    auto future2 = std::async(std::launch::async, []() { busyWork(30ms); });
    future1.wait();
    future2.wait();
    const auto elapsed = timer.elapsed<std::chrono::milliseconds>();
    const int expectedRealTime = 30;
    REQUIRE_THAT(elapsed.realTime.count(),
                 AtLeast(expectedRealTime - 1, tolerance));
    // Work happened twice
    REQUIRE_THAT(elapsed.processTime.count(),
                 AtLeast((expectedRealTime - 1) * 2, tolerance));
    // Work happened on different threads:
    REQUIRE_THAT(elapsed.threadTime.count(), AtLeast(0, tolerance));
}
