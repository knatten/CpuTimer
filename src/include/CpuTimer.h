#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iostream>

#ifdef KNATTEN_CPUTIMER_YOLO_ABI_BREAKING
#define KNATTEN_CPUTIMER_YOLO
#endif

#ifdef KNATTEN_CPUTIMER_YOLO
constexpr bool is_noexcept = true;
#else
constexpr bool is_noexcept = false;
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

        template <Type ClockType> void checked_clock_gettime(timespec &out)
        {
#ifndef KNATTEN_CPUTIMER_YOLO
            const auto result =
#endif
                clock_gettime(Detail::TypeTrait<ClockType>::type, &out);
#ifndef KNATTEN_CPUTIMER_YOLO
            if (result == -1)
            {
                throw std::runtime_error("Failed to get time: errno " +
                                         std::to_string(errno));
            }
#endif
        }

        template <Type ClockType, typename Duration = std::chrono::nanoseconds>
        Duration timeSince(const timespec &startTime)
        {
            timespec now;
            Detail::checked_clock_gettime<ClockType>(now);
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
        void start() noexcept(is_noexcept)
        {
#ifndef KNATTEN_CPUTIMER_YOLO_ABI_BREAKING
            state = Detail::State::started;
#endif
            Detail::checked_clock_gettime<ClockType>(startTime);
        }

        // Get elapsed time.
        // Throws: std::runtime_error if the timer was not started.
        template <typename Duration = std::chrono::nanoseconds>
        Duration elapsed() const noexcept(is_noexcept)
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
#ifndef KNATTEN_CPUTIMER_YOLO_ABI_BREAKING
        Detail::State state{Detail::State::notStarted};
#endif

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
        void start() noexcept(is_noexcept)
        {
#ifndef KNATTEN_CPUTIMER_YOLO_ABI_BREAKING
            state = Detail::State::started;
#endif
            Detail::checked_clock_gettime<Type::real>(realStartTime);
            Detail::checked_clock_gettime<Type::process>(processStartTime);
            Detail::checked_clock_gettime<Type::thread>(threadStartTime);
        }

        // Get elapsed time.
        // Throws: std::runtime_error if the timer was not started.
        template <typename Duration = std::chrono::nanoseconds>
        Result<Duration> elapsed() const noexcept(is_noexcept)
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
#ifndef KNATTEN_CPUTIMER_YOLO_ABI_BREAKING
        Detail::State state{Detail::State::notStarted};
#endif

        friend void fakeStartTimeNow(Timer &);
    };

} // namespace knatten::CpuTimer
