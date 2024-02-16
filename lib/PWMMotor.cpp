#include "framework.h"
#include "PWMMotor.h"

PWMMotor::PWMMotor(float cycleFrequency)
{
    speed = 0.0f;
    pwmDuty = 0;
    cycleTime = 1.0f / cycleFrequency;
}

void PWMMotor::set(float speed)
{
    this->speed = speed;
    this->pwmDuty = (uint16_t)floor(fmin(1.0f, fabs(speed)) * (float)UINT16_MAX);
}

float PWMMotor::getSpeed()
{
    return this->speed;
}

void PWMMotor::update(Motor motor, float timestamp)
{
    float cycleCount = timestamp / cycleTime;
    float cycleDelta = cycleCount - floor(cycleCount);
    uint16_t cycleDeltaInt = floor(cycleDelta * UINT16_MAX);
    if (cycleDeltaInt < this->pwmDuty)
    {
        if (this->speed > 0)
            motor.clockwise();
        else
            motor.counterclockwise();
    }
    else
    {
        motor.stop();
    }
}