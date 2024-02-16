#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include "framework.h"
#include "PWMMotor.h"
#include "clock.h"

typedef struct PIDController
{
public:
    PIDController(Clock *clock, float P, float threshold);
    void setTarget(float value);
    float calculate(float current);
    bool atTarget(float current);

private:
    Clock *clock;
    float target;
    float P;
    float threshold;

    unsigned long prevCount;
} PIDController;

#endif