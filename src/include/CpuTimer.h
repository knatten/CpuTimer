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
    namespace Detail
    {
        enum class State : uint8_t
        {
            notStarted,
            started
        };
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
        // Start the timer.
        // Can be called multiple times, which restarts the timer.
        void start()
        {
            state = Detail::State::started;
            clock_gettime(Detail::TypeTrait<ClockType>::type, &startTime);
        }

        // Get elapsed time.
        // Throws: std::runtime_error if the timer was not started.
        template <typename Duration = std::chrono::nanoseconds>
        Duration elapsed() const
        {
            if (state == Detail::State::notStarted)
            {
                throw std::runtime_error("Trying to get elapsed time of a "
                                         "timer which was not started");
            }
            timespec now;
            clock_gettime(Detail::TypeTrait<ClockType>::type, &now);
            const auto ns{(now.tv_sec - startTime.tv_sec) * 1000000000 +
                          (now.tv_nsec - startTime.tv_nsec)};
            return std::chrono::duration_cast<Duration>(
                std::chrono::nanoseconds{ns});
        }

      private:
        timespec startTime;
        Detail::State state{Detail::State::notStarted};
    };

    using RealTimer = SingleTimer<Type::real>;
    using ProcessTimer = SingleTimer<Type::process>;
    using ThreadTimer = SingleTimer<Type::thread>;

    class Timer
    {
      public:
        template <typename Duration> struct Result
        {
            Duration realTime;
            Duration processTime;
            Duration threadTime;
        };

        // Start the timer.
        // Can be called multiple times, which restarts the timer.
        void start()
        {
            realTimer.start();
            processTimer.start();
            threadTimer.start();
        }

        // Get elapsed time.
        // Throws: std::runtime_error if the timer was not started.
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
