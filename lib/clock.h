#ifndef CLOCK_H
#define CLOCK_H

/// @brief Clock provides basic timestamps based on a timer. It is best to only have one Clock object and pass it as a reference to functions that need it.
typedef struct
{
    /// @brief Initialize the clock timer (only call once in program)
    void init();

    /// @brief Returns the raw tick value of the counter
    unsigned long counter();

    /// @brief Returns the current time in microseconds
    float micros();

    /// @brief Converts a tick count into microseconds (for use with delta times)
    float toMicros(unsigned long count);

    /// @brief Returns the current time in seconds
    float seconds();

    /// @brief Converts a tick count into seconds (for use with delta times)
    float toSeconds(unsigned long count);
} Clock;

#endif