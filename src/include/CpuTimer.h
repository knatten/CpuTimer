#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iostream>

#ifndef KNATTEN_CPUTIMER_YOLO
#include <stdexcept>
#endif

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

        template <Type ClockType, typename Duration = std::chrono::nanoseconds>
        Duration timeSince(const timespec &startTime)
        {
            timespec now;
            clock_gettime(Detail::TypeTrait<ClockType>::type, &now);
            const auto ns{(now.tv_sec - startTime.tv_sec) * 1000000000 +
                          (now.tv_nsec - startTime.tv_nsec)};
            return std::chrono::duration_cast<Duration>(
                std::chrono::nanoseconds{ns});
        }

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
#ifndef KNATTEN_CPUTIMER_YOLO
            if (state == Detail::State::notStarted)
            {
                throw std::runtime_error("Trying to get elapsed time of a "
                                         "timer which was not started");
            }
#endif
            return Detail::timeSince<ClockType, Duration>(startTime);
        }

      private:
        timespec startTime;
        Detail::State state{Detail::State::notStarted};

        template <Type T> friend void fakeStartTimeNow(SingleTimer<T> &);
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
            state = Detail::State::started;
            clock_gettime(Detail::TypeTrait<Type::real>::type, &realStartTime);
            clock_gettime(Detail::TypeTrait<Type::process>::type,
                          &processStartTime);
            clock_gettime(Detail::TypeTrait<Type::thread>::type,
                          &threadStartTime);
        }

        // Get elapsed time.
        // Throws: std::runtime_error if the timer was not started.
        template <typename Duration = std::chrono::nanoseconds>
        Result<Duration> elapsed() const
        {
#ifndef KNATTEN_CPUTIMER_YOLO
            if (state == Detail::State::notStarted)
            {
                throw std::runtime_error("Trying to get elapsed time of a "
                                         "timer which was not started");
            }
#endif
            return Result<Duration>{
                Detail::timeSince<Type::real, Duration>(realStartTime),
                Detail::timeSince<Type::process, Duration>(processStartTime),
                Detail::timeSince<Type::thread, Duration>(threadStartTime)};
        }

      private:
        // Do not reuse SingleTimers here, to avoid triplicated if checks
        // Instead live with some duplication
        timespec realStartTime;
        timespec processStartTime;
        timespec threadStartTime;
        Detail::State state{Detail::State::notStarted};

        friend void fakeStartTimeNow(Timer &);
    };

} // namespace knatten::CpuTimer
