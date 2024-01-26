#ifndef _DRIVETRAIN_H_
#define _DRIVETRAIN_H_

enum class Direction
{
    Forward,
    Backward
};

typedef struct
{
    void enable();
    void disable();
    void setVelocity(float left, float right);
    void drive(Direction direction);

    void requestUpdate();

    float getLeftPower();
    float getRightPower();
    uint8_t currentCommand();
    bool isBusy();
    float getLeftVelocity();
    float getRightVelocity();
    float getTurnVelocity();
    float getAngle();

private:
    float currentLeftPower;
    float currentRightPower;
    uint8_t currentCommandId;
    float leftVelocity;
    float rightVelocity;
    float turnVelocity;
    float currentAngle;
} Drivetrain;

#endif