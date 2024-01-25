#include "framework.h"
#include "clock.h"

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

float Clock::micros()
{
    return counter() / 2.0f;
}