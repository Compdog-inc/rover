#include "framework.h"
#include "clock.h"

Time Time::fromTicks(unsigned long ticks)
{
    Time time = {};
    time.precision = TimePrecision::Ticks;
    time.count = ticks;
    return time;
}

Time Time::fromMicros(float micros)
{
    Time time = {};
    time.precision = TimePrecision::Micros;
    time.micros = micros;
    return time;
}

Time Time::fromSeconds(float seconds)
{
    Time time = {};
    time.precision = TimePrecision::Seconds;
    time.seconds = seconds;
    return time;
}

Time Time::between(Time &a, Time &b)
{
    TimePrecision highestPrecision = a.precision > b.precision ? a.precision : b.precision;
    switch (highestPrecision)
    {
    case TimePrecision::Ticks:
    {
        auto left = a.asTicks();
        auto right = b.asTicks();
        return Time::fromTicks(left > right ? left - right : right - left);
    }
    case TimePrecision::Micros:
    {
        auto left = a.asMicros();
        auto right = b.asMicros();
        return Time::fromMicros(left > right ? left - right : right - left);
    }
    case TimePrecision::Seconds:
    {
        auto left = a.asSeconds();
        auto right = b.asSeconds();
        return Time::fromSeconds(left > right ? left - right : right - left);
    }
    default:
        return {};
    }
}

TimePrecision Time::getPrecision()
{
    return precision;
}

unsigned long Time::asTicks()
{
    switch (precision)
    {
    case TimePrecision::Ticks:
        return count;
    case TimePrecision::Micros:
        return Clock::fromMicros(micros);
    case TimePrecision::Seconds:
        return Clock::fromSeconds(seconds);
    default:
        return 0;
    }
}

float Time::asMicros()
{
    switch (precision)
    {
    case TimePrecision::Ticks:
        return Clock::toMicros(count);
    case TimePrecision::Micros:
        return micros;
    case TimePrecision::Seconds:
        return seconds * 1000000.0f;
    default:
        return 0;
    }
}

float Time::asSeconds()
{
    switch (precision)
    {
    case TimePrecision::Ticks:
        return Clock::toSeconds(count);
    case TimePrecision::Micros:
        return micros / 1000000.0f;
    case TimePrecision::Seconds:
        return seconds;
    default:
        return 0;
    }
}

int Time::compareTo(Time &other)
{
    TimePrecision lowestPrecision = precision < other.precision ? precision : other.precision;
    switch (lowestPrecision)
    {
    case TimePrecision::Ticks:
    {
        auto left = asTicks();
        auto right = other.asTicks();
        return left == right  ? 0
               : left < right ? -1
                              : 1;
    }
    case TimePrecision::Micros:
    {
        auto left = asMicros();
        auto right = other.asMicros();
        return left == right  ? 0
               : left < right ? -1
                              : 1;
    }
    case TimePrecision::Seconds:
    {
        auto left = asSeconds();
        auto right = other.asSeconds();
        return left == right  ? 0
               : left < right ? -1
                              : 1;
    }
    default:
        return 0;
    }
}

volatile unsigned long _counter = 0;

ISR(TIMER1_OVF_vect)
{
    _counter++;
}

void Clock::init()
{
    TCCR1A = (0 << WGM10) | (0 << WGM11);
    TCCR1B = (0 << CS10) | (1 << CS11) | (0 << CS12) | (0 << WGM10) | (0 << WGM12) | (0 << WGM13);
    TIMSK1 = (1 << TOIE1);
    sei();
}

unsigned long Clock::counter()
{
    uint8_t _SREG = SREG;
    cli();
    uint16_t tmrVal = TCNT1;
    if (TIFR1 & (1 << TOV1))
    {
        tmrVal = TCNT1;
        _counter++;
        TIFR1 |= (1 << TOV1);
    }
    SREG = _SREG;
    return _counter * 0xFFFF + tmrVal;
}

Time Clock::time()
{
    return Time::fromTicks(counter());
}

float Clock::micros()
{
    return counter() / 2.0f;
}

float Clock::toMicros(unsigned long count)
{
    return count / 2.0f;
}

unsigned long Clock::fromMicros(float micros)
{
    return (unsigned long)round(fabs(micros * 2.0f));
}

float Clock::toSeconds(unsigned long count)
{
    return toMicros(count) / 1000000.0f;
}

unsigned long Clock::fromSeconds(float seconds)
{
    return fromMicros(seconds * 1000000.0f);
}

float Clock::seconds()
{
    return micros() / 1000000.0f;
}