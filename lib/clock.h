#ifndef CLOCK_H
#define CLOCK_H

typedef struct
{
    void init();
    unsigned long counter();
    float micros();
    float toMicros(unsigned long count);
} Clock;

#endif