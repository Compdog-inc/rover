#include "framework.h"
#include "pidcontroller.h"

PIDController::PIDController(Clock *clock, float P, float threshold)
{
    this->target = 0.0f;
    this->P = P;
    this->threshold = threshold;
    this->clock = clock;
    this->prevCount = clock->counter();
}

void PIDController::setTarget(float value)
{
    this->target = value;
}

bool PIDController::atTarget(float current)
{
    return current >= target - threshold && current <= target + threshold;
}

float PIDController::calculate(float current)
{
    unsigned long count = clock->counter();
    float delta = clock->toSeconds(count - prevCount);
    prevCount = count;

    // calculate pid output
    float output = current + fmin(1.0f, fmax(-1.0f, (target - current))) * delta * P;

    // threshold output
    if (output >= target - threshold && output <= target + threshold)
    {
        return target;
    }

    return output;
}