#ifndef _DRIVETRAIN_H_
#define _DRIVETRAIN_H_

#include "clock.h"

enum class Direction
{
    Forward,
    Backward
};

typedef struct Drivetrain
{
    Drivetrain(Clock *clock);

    void enable();
    void disable();

    void setVelocity(float left, float right);
    void setTurnVelocity(float velocity);
    void drive(Direction direction);
    void stop();
    void turn(float angle);
    void move(float distance);

    bool requestUpdate();
    void logTelemetry();

    float getLeftPower();
    float getRightPower();
    uint8_t currentCommand();
    bool isBusy();
    void waitUntilAvailable();
    float getLeftVelocity();
    float getRightVelocity();
    float getTurnVelocity();
    float getAngle();

private:
    Clock *clock;

    float currentLeftPower;
    float currentRightPower;
    uint8_t currentCommandId;
    float leftVelocity;
    float rightVelocity;
    float turnVelocity;
    float currentAngle;

    float frontLeftSpeed;
    float frontRightSpeed;
    float centerLeftSpeed;
    float centerRightSpeed;
    float backLeftSpeed;
    float backRightSpeed;
} Drivetrain;

#endif