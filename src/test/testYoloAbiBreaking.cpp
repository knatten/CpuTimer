#define KNATTEN_CPUTIMER_YOLO_ABI_BREAKING
#include "CpuTimer.h"

#include <catch2/catch_all.hpp>

#include <chrono>
#include <thread>

using namespace knatten::CpuTimer;

using Catch::Matchers::WithinAbs;
using namespace std::chrono_literals;

static_assert(sizeof(RealTimer) == sizeof(timespec));
static_assert(sizeof(Timer) == 3 * sizeof(timespec));

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
