#ifndef TIMER_H
#define TIMER_H

#include "clock.h"

/// @brief Provides functions for getting elapsed time and comparing time
typedef struct Timer
{
public:
    Timer(Clock *clock);
    Time elapsed();
    bool elapsed(Time &time);
    void stop();
    void start();
    void restart();
    void reset();
    bool running();

    /// @brief Performs a spin wait until the target time has elapsed
    /// @note This restarts the timer
    void spinWait(Time time);
    void spinWait(Time &time);

private:
    Clock *clock;
    unsigned long c_start;
    unsigned long c_end;
    bool isRunning;
} Timer;

#endif