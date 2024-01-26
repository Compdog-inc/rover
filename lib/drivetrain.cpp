#include "framework.h"
#include "drivetrain.h"
#include "i2c.h"
#include "constants.h"

void Drivetrain::enable()
{
    TWI::enable();
}

void Drivetrain::disable()
{
    TWI::disable();
}

void Drivetrain::setVelocity(float left, float right)
{
}

void Drivetrain::drive(Direction direction)
{
    uint8_t cmd[2];
    cmd[0] = CMD_DRIVETRAIN_DRIVE; // id
    cmd[1] = direction == Direction::Forward ? DRIVETRAIN_DIRECTION_FORWARD : DRIVETRAIN_DIRECTION_BACKWARD;

    TWI::sendTo(DRIVETRAIN_I2C);
    TWI::write(cmd, 2);
    TWI::endTransfer();
}

void encodeFloat(uint8_t *buf, float f)
{
    buf[0] = (uint32_t)f & 0xFF;
    buf[1] = ((uint32_t)f >> 8) & 0xFF;
    buf[2] = ((uint32_t)f >> 16) & 0xFF;
    buf[3] = ((uint32_t)f >> 24) & 0xFF;
}

float decodeFloat(uint8_t *buf)
{
    return (float)(((uint32_t)buf[0]) & ((uint32_t)buf[1] << 8) & ((uint32_t)buf[2] << 16) & ((uint32_t)buf[3] << 24));
}

void Drivetrain::requestUpdate()
{
    uint8_t buf[4];

    TWI::requestFrom(DRIVETRAIN_I2C);

    // read the current left and right drivetrain power (current PWM value of motors)
    if (TWI::read(buf, 4) != 4)
        return;
    currentLeftPower = decodeFloat(buf);

    if (TWI::read(buf, 4) != 4)
        return;
    currentRightPower = decodeFloat(buf);

    // read the current command id
    if (TWI::read(buf, 1) != 1)
        return;
    currentCommandId = buf[0];

    // read the left and right drivetrain velocity (set by master)
    if (TWI::read(buf, 4) != 4)
        return;
    leftVelocity = decodeFloat(buf);

    if (TWI::read(buf, 4) != 4)
        return;
    rightVelocity = decodeFloat(buf);

    // read the turn velocity (set by master)
    if (TWI::read(buf, 4) != 4)
        return;
    turnVelocity = decodeFloat(buf);

    // read the current angle
    if (TWI::read(buf, 4) != 4)
        return;
    currentAngle = decodeFloat(buf);

    TWI::endTransfer();
}

float Drivetrain::getLeftPower()
{
    return currentLeftPower;
}

float Drivetrain::getRightPower()
{
    return currentRightPower;
}

uint8_t Drivetrain::currentCommand()
{
    return currentCommandId;
}

bool Drivetrain::isBusy()
{
    return currentCommandId != CMD_NONE;
}

float Drivetrain::getLeftVelocity()
{
    return leftVelocity;
}

float Drivetrain::getRightVelocity()
{
    return rightVelocity;
}

float Drivetrain::getTurnVelocity()
{
    return turnVelocity;
}

float Drivetrain::getAngle()
{
    return currentAngle;
}