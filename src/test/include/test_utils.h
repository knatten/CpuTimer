#pragma once

#include "CpuTimer.h"

namespace knatten::CpuTimer
{
    template <Type ClockType>
    void fakeStartTimeNow(SingleTimer<ClockType> &timer)
    {
        timespec fakeStartTime;
        clock_gettime(CLOCK_REALTIME, &fakeStartTime);
        timer.startTime = fakeStartTime;
    }

    void fakeStartTimeNow(Timer &timer)
    {
        timespec fakeStartTime;
        clock_gettime(CLOCK_REALTIME, &fakeStartTime);
        timer.realStartTime = fakeStartTime;
        timer.processStartTime = fakeStartTime;
        timer.threadStartTime = fakeStartTime;
    }
} // namespace knatten::CpuTimer
