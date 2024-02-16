#ifndef MOTOR_H
#define MOTOR_H

#include "framework.h"

#define BRAKE_MODE false

typedef struct Motor
{
public:
    /// @brief Initializes a two-pin DC motor
    /// @param pin1 The first pin of the motor (positive when clockwise)
    /// @param pin2 The second pin of the motor (positive when counterclockwise)
    Motor(uint8_t pin1, uint8_t pin2);

    /// @brief Spins the motor clockwise
    void clockwise();

    /// @brief Spins the motor counterclockwise
    void counterclockwise();

    /// @brief Stops the motor
    void stop();

private:
    uint8_t pin1;
    uint8_t pin2;
} Motor;

#endif