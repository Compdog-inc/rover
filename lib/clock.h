#ifndef CLOCK_H
#define CLOCK_H

enum class TimePrecision
{
    Seconds = 0,
    Micros = 1,
    Ticks = 2
};

typedef struct Time
{
public:
    static Time fromTicks(unsigned long ticks);
    static Time fromMicros(float micros);
    static Time fromSeconds(float seconds);
    /// @brief Returns the absolute time between two absolute timestamps
    static Time between(Time &a, Time &b);

    TimePrecision getPrecision();
    unsigned long asTicks();
    float asMicros();
    float asSeconds();
    int compareTo(Time &other);

private:
    union
    {
        unsigned long count;
        float micros;
        float seconds;
    };
    TimePrecision precision;
} Time;

/// @brief Clock provides basic timestamps based on a timer. It is best to only have one Clock object and pass it as a reference to functions that need it.
typedef struct
{
    /// @brief Initialize the clock timer (only call once in program)
    void init();

    /// @brief Returns the raw tick value of the counter
    unsigned long counter();
    Time time();

    /// @brief Returns the current time in microseconds
    float micros();

    /// @brief Converts a tick count into microseconds (for use with delta times)
    static float toMicros(unsigned long count);

    /// @brief Converts microseconds info a tick count (for use with delta times)
    static unsigned long fromMicros(float micros);

    /// @brief Returns the current time in seconds
    float seconds();

    /// @brief Converts a tick count into seconds (for use with delta times)
    static float toSeconds(unsigned long count);

    /// @brief Converts seconds info a tick count (for use with delta times)
    static unsigned long fromSeconds(float seconds);
} Clock;

#endif