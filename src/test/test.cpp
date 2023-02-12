#include "CpuTimer.h"

#include <catch2/catch_all.hpp>

#include <chrono>
#include <thread>

using Catch::Matchers::Equals;

using namespace knatten::CpuTimer;
using namespace std::chrono_literals;

TEST_CASE("Single timer")
{
    SingleTimer<Type::real> realTimer;
    realTimer.start();

    // Lap time
    std::this_thread::sleep_for(10ms);
    const auto lapTime = realTimer.elapsed();
    REQUIRE(lapTime > 0ms);
    REQUIRE(lapTime < 20ms);

    // Stop time
    std::this_thread::sleep_for(10ms);
    realTimer.stop();
    const auto stopTime = realTimer.elapsed();
    REQUIRE(stopTime > lapTime);
    REQUIRE(stopTime <
            1000ms); // Ridiculous error margin to avoid flaky tests in CI

    // After stopping, elapsed stays the same
    std::this_thread::sleep_for(10ms);
    REQUIRE(stopTime == realTimer.elapsed());
}

TEST_CASE("Stopping without starting throws")
{
    SingleTimer<Type::real> timer;
    REQUIRE_THROWS_WITH(timer.stop(),
                        Equals("Trying to stop a timer which was not started"));
}
TEST_CASE("Checking elapsed time without starting throws")
{
    SingleTimer<Type::real> timer;
    REQUIRE_THROWS_WITH(timer.elapsed(),
                        Equals("Trying to stop a timer which was not started"));
}
