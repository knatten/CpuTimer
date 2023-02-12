#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <optional>

namespace knatten::CpuTimer
{
    enum class Type
    {
        real,
        process,
        thread
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
            if (!startTime)
            {
                startTime.template emplace();
            }
            clock_gettime(Detail::TypeTrait<ClockType>::type, &(*startTime));
        }

        // Throws: std::runtime_error if the clock was not started
        void stop()
        {
            assertStarted();
            stopTime.template emplace();
            clock_gettime(Detail::TypeTrait<ClockType>::type, &(*stopTime));
        }

        // Get elapsed time.
        // If the timer was not previously stopped, returns the elapsed "lap"
        // time until the current time. If the timer was previously stopped,
        // returns the time elapsed until the time when it was stopped.
        // Throws: std::runtime_error if the timer was not started
        template <typename Duration = std::chrono::nanoseconds>
        Duration elapsed() const
        {
            assertStarted();
            const auto until = [this]()
            {
                if (stopTime.has_value())
                {
                    return *stopTime;
                }
                else
                {
                    timespec ts;
                    clock_gettime(Detail::TypeTrait<ClockType>::type, &ts);
                    return ts;
                }
            }();
            const auto ns{(until.tv_sec - startTime->tv_sec) * 1000000000 +
                          (until.tv_nsec - startTime->tv_nsec)};
            return std::chrono::duration_cast<Duration>(
                std::chrono::nanoseconds{ns});
        }

      private:
        std::optional<timespec> startTime;
        std::optional<timespec> stopTime;

        void assertStarted() const
        {
            if (!startTime)
            {
                throw std::runtime_error(
                    "Trying to stop a timer which was not started");
            }
        }
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
