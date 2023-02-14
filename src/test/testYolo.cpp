#define KNATTEN_CPUTIMER_YOLO
#include "CpuTimer.h"
#include "test_utils.h"

#include <catch2/catch_all.hpp>

#include <chrono>
#include <thread>

using namespace knatten::CpuTimer;

using Catch::Matchers::WithinAbs;
using namespace std::chrono_literals;

static_assert(noexcept(RealTimer{}.start()));
static_assert(noexcept(RealTimer{}.elapsed()));
static_assert(noexcept(Timer{}.start()));
static_assert(noexcept(Timer{}.elapsed()));

TEST_CASE("Checking elapsed time of non-started SingleTimer does not throw")
{
    RealTimer realTimer;
    fakeStartTimeNow(realTimer); // Avoid signed int overflow on next line
    realTimer.elapsed(); // Should not throw now, but can't use REQUIRE_NOTHROW
                         // with exceptions disabled
}

TEST_CASE("Checking elapsed time of non-started T does not throw")
{
    Timer timer;
    fakeStartTimeNow(timer); // Avoid signed int overflow on next line
    timer.elapsed(); // Should not throw now, but can't use REQUIRE_NOTHROW with
                     // exceptions disabled
}

TEST_CASE("Smoke test that SingleTimer still works")
{
    RealTimer realTimer;
    realTimer.start();

    std::this_thread::sleep_for(20ms);
    const auto lapTime = realTimer.elapsed<std::chrono::milliseconds>();
    REQUIRE_THAT(lapTime.count(), WithinAbs(30, 10));
}

TEST_CASE("Smoke test that Timer still works")
{
    Timer realTimer;
    realTimer.start();

    std::this_thread::sleep_for(20ms);
    const auto lapTime = realTimer.elapsed<std::chrono::milliseconds>();
    REQUIRE_THAT(lapTime.realTime.count(), WithinAbs(30, 10));
}
