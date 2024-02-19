#include "framework.h"
#include "timer.h"

Timer::Timer(Clock *clock)
{
    this->clock = clock;
    this->isRunning = true;
    this->c_start = clock->counter();
    this->c_end = c_start;
}

Time Timer::elapsed()
{
    if (isRunning)
        return Time::fromTicks(clock->counter() - c_start);
    else
        return Time::fromTicks(c_end - c_start);
}

bool Timer::elapsed(Time &time)
{
    Time t = elapsed();
    return t.compareTo(time) >= 0;
}

void Timer::stop()
{
    if (isRunning)
    {
        isRunning = false;
        c_end = clock->counter();
    }
}

void Timer::start()
{
    if (!isRunning)
    {
        isRunning = true;
        c_start = clock->counter();
        c_end = c_start;
    }
}

void Timer::restart()
{
    isRunning = true;
    c_start = clock->counter();
    c_end = c_start;
}

void Timer::reset()
{
    c_start = clock->counter();
    c_end = c_start;
}

bool Timer::running()
{
    return isRunning;
}

void Timer::spinWait(Time &time)
{
    restart();
    while (!elapsed(time))
        ;
}

void Timer::spinWait(Time time)
{
    restart();
    while (!elapsed(time))
        ;
}