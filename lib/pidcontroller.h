#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include "framework.h"
#include "PWMMotor.h"
#include "timer.h"

typedef struct PIDController
{
public:
    /// @brief PID controller for any value
    /// @param clock Reference to a Clock object, used for the PID delta
    /// @param P P coefficient of the controller
    /// @param threshold Threshold between the current value and the target at which to snap exactly to the target (set to 0 to disable)
    PIDController(Clock *clock, float P, float threshold);

    /// @brief Sets the PID target value
    void setTarget(float value);
    /// @brief Returns the PID target value
    float getTarget();

    /// @brief Calculates a PID output based on the current value and the set target
    /// @param current The current value
    /// @return Returns the output from the controller
    float calculate(float current);

    /// @brief Returns true if the current value is within the threshold of the target
    bool atTarget(float current);

private:
    Timer timer;
    float target;
    float P;
    float threshold;
} PIDController;

#endif