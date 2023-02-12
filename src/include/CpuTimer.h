#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iostream>

namespace knatten::CpuTimer
{
    enum class Type
    {
        real,
        process,
        thread
    };
    enum class State : uint8_t
    {
        notStarted,
        started,
        stopped
    };
    namespace Detail
    {
        template <Type type> struct TypeTrait
        {
        };
        template <> struct TypeTrait<Type::real>
        {
            static constexpr auto type = CLOCK_REALTIME;
        };
        template <> struct TypeTrait<Type::process>
        {
            static constexpr auto type = CLOCK_PROCESS_CPUTIME_ID;
        };
        template <> struct TypeTrait<Type::thread>
        {
            static constexpr auto type = CLOCK_THREAD_CPUTIME_ID;
        };
    }; // namespace Detail

    template <Type ClockType> class SingleTimer
    {
      public:
        void start()
        {
            state = State::started;
            clock_gettime(Detail::TypeTrait<ClockType>::type, &startTime);
        }

        // Throws: std::runtime_error if the clock was not started
        void stop()
        {
            if (state == State::notStarted)
            {
                throw std::runtime_error(
                    "Trying to stop a timer which was not started");
            }
            state = State::stopped;
            clock_gettime(Detail::TypeTrait<ClockType>::type, &stopTime);
        }

        // Get elapsed time.
        // If the timer was not previously stopped, returns the elapsed "lap"
        // time until the current time. If the timer was previously stopped,
        // returns the time elapsed until the time when it was stopped.
        // Throws: std::runtime_error if the timer was not started
        template <typename Duration = std::chrono::nanoseconds>
        Duration elapsed() const
        {
            if (state == State::notStarted)
            {
                throw std::runtime_error(
                    "Trying to get elapsed time of a timer which was not started");
            }
            const auto until = [this]()
            {
                if (state == State::stopped)
                {
                    return stopTime;
                }
                else
                {
                    timespec now;
                    clock_gettime(Detail::TypeTrait<ClockType>::type, &now);
                    return now;
                }
            }();
            const auto ns{(until.tv_sec - startTime.tv_sec) * 1000000000 +
                          (until.tv_nsec - startTime.tv_nsec)};
            return std::chrono::duration_cast<Duration>(
                std::chrono::nanoseconds{ns});
        }

      private:
        timespec startTime;
        timespec stopTime;
        State state{State::notStarted};
    };

    using RealTimer = SingleTimer<Type::real>;
    using ProcessTimer = SingleTimer<Type::process>;
    using ThreadTimer = SingleTimer<Type::thread>;

    class Timer
    {
        template <typename Duration> struct Result
        {
            Duration realTime;
            Duration processTime;
            Duration threadTime;
        };

      public:
        void start()
        {
            realTimer.start();
            processTimer.start();
            threadTimer.start();
        }

        void stop()
        {
            realTimer.stop();
            processTimer.stop();
            threadTimer.stop();
        }

        template <typename Duration = std::chrono::nanoseconds>
        Result<Duration> elapsed() const
        {
            return Result<Duration>{realTimer.template elapsed<Duration>(),
                                    processTimer.template elapsed<Duration>(),
                                    threadTimer.template elapsed<Duration>()};
        }

      private:
        RealTimer realTimer;
        ProcessTimer processTimer;
        ThreadTimer threadTimer;
    };

} // namespace knatten::CpuTimer
