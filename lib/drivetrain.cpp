#include "framework.h"
#include "drivetrain.h"
#include "i2c.h"
#include "constants.h"
#include "serialdebug.h"
#include "timer.h"
#include "serialize.h"

DebugInterface dbgdrive("Drivetrain", Version(256));

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

void Drivetrain::stop()
{
    uint8_t cmd[1];
    cmd[0] = CMD_DRIVETRAIN_STOP; // id

    TWI::sendTo(DRIVETRAIN_I2C);
    TWI::write(cmd, 1);
    TWI::endTransfer();
}

void Drivetrain::requestUpdate()
{
    uint8_t buf[4];

    if (!TWI::requestFrom(DRIVETRAIN_I2C))
        return;

    // read the current motor speeds
    if (TWI::read(buf, 4) != 4)
        return;
    dbgdrive.array(buf, 4);
    frontLeftSpeed = decodeFloat(buf);
    if (TWI::read(buf, 4) != 4)
        return;
    frontRightSpeed = decodeFloat(buf);
    if (TWI::read(buf, 4) != 4)
        return;
    centerLeftSpeed = decodeFloat(buf);
    if (TWI::read(buf, 4) != 4)
        return;
    centerRightSpeed = decodeFloat(buf);
    if (TWI::read(buf, 4) != 4)
        return;
    backLeftSpeed = decodeFloat(buf);
    if (TWI::read(buf, 4) != 4)
        return;
    backRightSpeed = decodeFloat(buf);

    // read the current command id
    if (TWI::read(buf, 1) != 1)
        return;
    currentCommandId = buf[0];

    // read the left and right drivetrain power
    if (TWI::read(buf, 4) != 4)
        return;
    currentLeftPower = decodeFloat(buf);
    if (TWI::read(buf, 4) != 4)
        return;
    currentRightPower = decodeFloat(buf);

    // read the current angle
    if (TWI::read(buf, 4) != 4)
        return;
    currentAngle = decodeFloat(buf);

    TWI::endTransfer();
}

void Drivetrain::logTelemetry()
{
    char buf[20];

    ftoa(currentLeftPower, buf, 20, 4);
    dbgdrive.info("leftPower: %s\n", buf);
    ftoa(currentRightPower, buf, 20, 4);
    dbgdrive.info("rightPower: %s\n", buf);

    dbgdrive.info("currentCommand: %u\n", currentCommandId);

    ftoa(currentAngle, buf, 20, 4);
    dbgdrive.info("angle: %s\n", buf);

    ftoa(frontLeftSpeed, buf, 20, 4);
    dbgdrive.info("frontLeftSpeed: %s\n", buf);
    ftoa(frontRightSpeed, buf, 20, 4);
    dbgdrive.info("frontRightSpeed: %s\n", buf);
    ftoa(centerLeftSpeed, buf, 20, 4);
    dbgdrive.info("centerLeftSpeed: %s\n", buf);
    ftoa(centerRightSpeed, buf, 20, 4);
    dbgdrive.info("centerRightSpeed: %s\n", buf);
    ftoa(backLeftSpeed, buf, 20, 4);
    dbgdrive.info("backLeftSpeed: %s\n", buf);
    ftoa(backRightSpeed, buf, 20, 4);
    dbgdrive.info("backRightSpeed: %s\n", buf);
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