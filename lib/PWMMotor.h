#ifndef PWM_MOTOR_H
#define PWM_MOTOR_H

#include "framework.h"
#include "motor.h"

typedef struct PWMMotor
{
public:
    PWMMotor(float cycleFrequency);
    void set(float speed);
    float getSpeed();
    void update(Motor motor, float timestamp);

private:
    float cycleTime;
    float speed;
    uint16_t pwmDuty;
} PWMMotor;

#endif