#ifndef PWM_MOTOR_H
#define PWM_MOTOR_H

#include "framework.h"
#include "motor.h"

typedef struct PWMMotor
{
public:
    /// @brief A PWM speed controller
    /// @param cycleFrequency The frequency in Hz of the PWM cycle
    PWMMotor(float cycleFrequency);

    /// @brief Sets the speed of the motor
    void set(float speed);

    /// @brief Returns the current speed of the motor
    float getSpeed();

    /// @brief Spins a motor depending on the target PWM speed
    /// @param motor The target motor to spin
    /// @param timestamp The current absolute timestamp in seconds
    void update(Motor motor, float timestamp);

private:
    float cycleTime;
    float speed;
    uint16_t pwmDuty;
} PWMMotor;

#endif