#ifndef MOTOR_H
#define MOTOR_H

#include "framework.h"

#define BRAKE_MODE false

typedef struct Motor
{
public:
    Motor(uint8_t pin1, uint8_t pin2);
    void clockwise();
    void counterclockwise();
    void stop();

private:
    uint8_t pin1;
    uint8_t pin2;
} Motor;

#endif