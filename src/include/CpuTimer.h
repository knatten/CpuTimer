#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iostream>

#include <stdexcept>

namespace knatten
{
    namespace CpuTimer
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

            template <Type ClockType> void checked_clock_gettime(timespec &time)
            {
                const auto result =
                    clock_gettime(Detail::TypeTrait<ClockType>::type, &time);
                if (result == -1)
                {
                    throw std::runtime_error("Failed to get time");
                }
            }

            template <Type ClockType,
                      typename Duration = std::chrono::nanoseconds>
            Duration timeSince(const timespec &startTime)
            {
                // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
                timespec now;
                checked_clock_gettime<ClockType>(now);
                const auto nanoseconds{(now.tv_sec - startTime.tv_sec) *
                                           1000000000 +
                                       (now.tv_nsec - startTime.tv_nsec)};
                return std::chrono::duration_cast<Duration>(
                    std::chrono::nanoseconds{nanoseconds});
            }

        }; // namespace Detail

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
        template <Type ClockType> class SingleTimer
        {
          public:
            // Start the timer.
            // Can be called multiple times, which restarts the timer.
            void start()
            {
                state = Detail::State::started;
                Detail::checked_clock_gettime<ClockType>(startTime);
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

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
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
                Detail::checked_clock_gettime<Type::real>(realStartTime);
                Detail::checked_clock_gettime<Type::process>(processStartTime);
                Detail::checked_clock_gettime<Type::thread>(threadStartTime);
            }

            // Get elapsed time.
            // Throws: std::runtime_error if the timer was not started.
            template <typename Duration = std::chrono::nanoseconds>
            Result<Duration> elapsed() const
            {
                if (state == Detail::State::notStarted)
                {
                    throw std::runtime_error("Trying to get elapsed time of a "
                                             "timer which was not started");
                }
                return Result<Duration>{
                    Detail::timeSince<Type::real, Duration>(realStartTime),
                    Detail::timeSince<Type::process, Duration>(
                        processStartTime),
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

    } // namespace CpuTimer
} // namespace knatten
