#include "framework.h"
#include "pidcontroller.h"

PIDController::PIDController(Clock *clock, float P, float threshold)
    : timer(clock)
{
    this->target = 0.0f;
    this->P = P;
    this->threshold = threshold;
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
    // Get elapsed time since last calculate
    Time delta = timer.elapsed();
    timer.reset();

    // calculate pid output
    float output = current + fmin(1.0f, fmax(-1.0f, (target - current))) * delta.asSeconds() * P;

    // threshold output
    if (output >= target - threshold && output <= target + threshold)
    {
        return target;
    }

    return output;
}