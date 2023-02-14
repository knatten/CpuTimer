#include "CpuTimer.h"

#include <catch2/catch_all.hpp>

#include <chrono>
#include <cmath>
#include <future>
#include <thread>

using Catch::Matchers::Equals;
using Catch::Matchers::WithinAbs;

using namespace knatten::CpuTimer;
using namespace std::chrono_literals;

static_assert(!noexcept(RealTimer{}.start()));
static_assert(!noexcept(RealTimer{}.elapsed()));
static_assert(!noexcept(Timer{}.start()));
static_assert(!noexcept(Timer{}.elapsed()));

// Only tests the interface of SingleTimer
// The particularities of real/process/thread are tested in the Timer tests
TEST_CASE("Single timer")
{
    RealTimer realTimer;
    realTimer.start();

    // Elapsed time
    std::this_thread::sleep_for(20ms);
    const auto lapTime = realTimer.elapsed<std::chrono::milliseconds>();
    REQUIRE_THAT(lapTime.count(), WithinAbs(30, 10));

    // Starting again re-starts the timer
    realTimer.start();
    REQUIRE_THAT(realTimer.elapsed<std::chrono::milliseconds>().count(),
                 WithinAbs(10, 10));
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
    REQUIRE_THAT(lapTime.realTime.count(), WithinAbs(30, 10));
    // Not affected by sleep:
    REQUIRE_THAT(lapTime.processTime.count(), WithinAbs(10, 10));
    // Not affected by sleep:
    REQUIRE_THAT(lapTime.threadTime.count(), WithinAbs(10, 10));

    // Starting again re-starts the timer
    timer.start();
    const auto restartedTime = timer.elapsed<std::chrono::milliseconds>();
    REQUIRE_THAT(restartedTime.realTime.count(), WithinAbs(10, 10));
    // Not affected by sleep:
    REQUIRE_THAT(restartedTime.processTime.count(), WithinAbs(10, 10));
    // Not affected by sleep:
    REQUIRE_THAT(restartedTime.threadTime.count(), WithinAbs(10, 10));
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
    REQUIRE_THAT(elapsed.realTime.count(), WithinAbs(30, 11));
    REQUIRE_THAT(elapsed.processTime.count(), WithinAbs(30, 11));
    REQUIRE_THAT(elapsed.threadTime.count(), WithinAbs(30, 11));
}

TEST_CASE("Full timer with busywork on separate thread")
{
    Timer timer;
    timer.start();
    auto future = std::async(std::launch::async, []() { busyWork(20ms); });
    future.wait();
    const auto elapsed = timer.elapsed<std::chrono::milliseconds>();
    REQUIRE_THAT(elapsed.realTime.count(), WithinAbs(30, 11));
    REQUIRE_THAT(elapsed.processTime.count(), WithinAbs(30, 11));
    // Work happened on different thread:
    REQUIRE_THAT(elapsed.threadTime.count(), WithinAbs(10, 10));
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
                 WithinAbs(expectedRealTime + 10, 11));
    // Work happened twice
    REQUIRE_THAT(elapsed.processTime.count(),
                 WithinAbs(expectedRealTime * 2 + 10, 11));
    // Work happened on different threads:
    REQUIRE_THAT(elapsed.threadTime.count(), WithinAbs(10, 10));
}
