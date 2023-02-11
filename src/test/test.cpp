#include "CpuClock.h"

#include <catch2/catch_all.hpp>

#include <chrono>
#include <thread>

using Catch::Matchers::Equals;

using namespace CpuClock;
using namespace std::chrono_literals;

TEST_CASE("Single timer")
{
    SingleTimer<Type::real> realClock;
    realClock.start();

    // Lap time
    std::this_thread::sleep_for(10ms);
    const auto lapTime = realClock.elapsed();
    REQUIRE(lapTime > 0ms);
    REQUIRE(lapTime < 20ms);

    // Stop time
    std::this_thread::sleep_for(10ms);
    realClock.stop();
    const auto stopTime = realClock.elapsed();
    REQUIRE(stopTime > lapTime);
    REQUIRE(stopTime <
            1000ms); // Ridiculous error margin to avoid flaky tests in CI

    // After stopping, elapsed stays the same
    std::this_thread::sleep_for(10ms);
    REQUIRE(stopTime == realClock.elapsed());
}

TEST_CASE("Stopping without starting throws")
{
    SingleTimer<Type::real> timer;
    REQUIRE_THROWS_WITH(timer.stop(),
                        Equals("Trying to stop a clock which was not started"));
}
TEST_CASE("Checking elapsed time without starting throws")
{
    SingleTimer<Type::real> timer;
    REQUIRE_THROWS_WITH(timer.elapsed(),
                        Equals("Trying to stop a clock which was not started"));
}
