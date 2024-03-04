#include "framework.h"
#include "drivetrain.h"
#include "i2c.h"
#include "constants.h"
#include "serialdebug.h"
#include "timer.h"
#include "serialize.h"
#include "serialterminal.h"

DebugInterface dbgdrive("Drivetrain", Version(256));
ByteStream i2c;

Drivetrain::Drivetrain(Clock *clock)
{
    this->clock = clock;
}

void Drivetrain::enable()
{
    TWI::enable();
    i2c = TWI::getStream();
}

void Drivetrain::disable()
{
    TWI::disable();
}

void Drivetrain::setVelocity(float left, float right)
{
    uint8_t cmd[9];
    cmd[0] = CMD_DRIVETRAIN_SET_VELOCITY; // id
    encodeFloat(cmd + 1, left);
    encodeFloat(cmd + 5, right);

    if (!TWI::sendTo(DRIVETRAIN_I2C))
    {
        dbgdrive.error("setVelocity error addressing device\n");
        return;
    }

    if (i2c.write(cmd, 0, 9) != 9)
    {
        dbgdrive.error("setVelocity error sending command\n");
        return;
    }

    TWI::endTransfer();

    leftVelocity = left;
    rightVelocity = right;
}

void Drivetrain::setTurnVelocity(float velocity)
{
    uint8_t cmd[5];
    cmd[0] = CMD_DRIVETRAIN_SET_TURN_VELOCITY; // id
    encodeFloat(cmd + 1, velocity);

    if (!TWI::sendTo(DRIVETRAIN_I2C))
    {
        dbgdrive.error("setTurnVelocity error addressing device\n");
        return;
    }

    if (i2c.write(cmd, 0, 5) != 5)
    {
        dbgdrive.error("setTurnVelocity error sending command\n");
        return;
    }

    TWI::endTransfer();

    turnVelocity = velocity;
}

void Drivetrain::drive(Direction direction)
{
    uint8_t cmd[2];
    cmd[0] = CMD_DRIVETRAIN_DRIVE; // id
    cmd[1] = direction == Direction::Forward ? DRIVETRAIN_DIRECTION_FORWARD : DRIVETRAIN_DIRECTION_BACKWARD;

    if (!TWI::sendTo(DRIVETRAIN_I2C))
    {
        dbgdrive.error("drive error addressing device\n");
        return;
    }

    if (i2c.write(cmd, 0, 2) != 2)
    {
        dbgdrive.error("drive error sending command\n");
        return;
    }

    TWI::endTransfer();
}

void Drivetrain::stop()
{
    uint8_t cmd[1];
    cmd[0] = CMD_DRIVETRAIN_STOP; // id

    if (!TWI::sendTo(DRIVETRAIN_I2C))
    {
        dbgdrive.error("stop error addressing device\n");
        return;
    }

    if (i2c.write(cmd, 0, 1) != 1)
    {
        dbgdrive.error("stop error sending command\n");
        return;
    }

    TWI::endTransfer();
}

void Drivetrain::turn(float angle)
{
    uint8_t cmd[5];
    cmd[0] = CMD_DRIVETRAIN_TURN; // id
    encodeFloat(cmd + 1, angle);

    if (!TWI::sendTo(DRIVETRAIN_I2C))
    {
        dbgdrive.error("turn error addressing device\n");
        return;
    }

    if (i2c.write(cmd, 0, 5) != 5)
    {
        dbgdrive.error("turn error sending command\n");
        return;
    }

    TWI::endTransfer();
}

void Drivetrain::move(float distance)
{
    uint8_t cmd[5];
    cmd[0] = CMD_DRIVETRAIN_MOVE; // id
    encodeFloat(cmd + 1, distance);

    if (!TWI::sendTo(DRIVETRAIN_I2C))
    {
        dbgdrive.error("move error addressing device\n");
        return;
    }

    if (i2c.write(cmd, 0, 5) != 5)
    {
        dbgdrive.error("move error sending command\n");
        return;
    }

    TWI::endTransfer();
}

bool Drivetrain::requestUpdate()
{
    uint8_t buf[4];

    if (!TWI::requestFrom(DRIVETRAIN_I2C))
        return false;

    // read the current motor speeds
    if (i2c.read(buf, 0, 4, true) != 4)
        return false;
    frontLeftSpeed = decodeFloat(buf);
    if (i2c.read(buf, 0, 4, true) != 4)
        return false;
    frontRightSpeed = decodeFloat(buf);
    if (i2c.read(buf, 0, 4, true) != 4)
        return false;
    centerLeftSpeed = decodeFloat(buf);
    if (i2c.read(buf, 0, 4, true) != 4)
        return false;
    centerRightSpeed = decodeFloat(buf);
    if (i2c.read(buf, 0, 4, true) != 4)
        return false;
    backLeftSpeed = decodeFloat(buf);
    if (i2c.read(buf, 0, 4, true) != 4)
        return false;
    backRightSpeed = decodeFloat(buf);

    // read the current command id
    if (i2c.read(buf, 0, 1, true) != 1)
        return false;
    currentCommandId = buf[0];

    // read the left and right drivetrain power
    if (i2c.read(buf, 0, 4, true) != 4)
        return false;
    currentLeftPower = decodeFloat(buf);
    if (i2c.read(buf, 0, 4, true) != 4)
        return false;
    currentRightPower = decodeFloat(buf);

    // read the current angle
    if (i2c.read(buf, 0, 4, false) != 4)
        return false;
    currentAngle = decodeFloat(buf);
    return true;
}

void Drivetrain::logTelemetry()
{
    char buf[20];

    // SerialTerminal::hideCursor();
    // SerialTerminal::moveCursor(1, 1);

    ftoa(69.0f, buf, 20, 4);
    dbgdrive.info("leftPower: %", buf);
    SerialTerminal::eraseFromCursorEndLine();
    SerialTerminal::moveCursorToNextLine(1);
    ftoa(currentRightPower, buf, 20, 4);
    dbgdrive.info("rightPower: %s", buf);
    SerialTerminal::eraseFromCursorEndLine();
    SerialTerminal::moveCursorToNextLine(1);
    dbgdrive.info("currentCommand: %u", currentCommandId);
    SerialTerminal::eraseFromCursorEndLine();
    SerialTerminal::moveCursorToNextLine(1);
    ftoa(currentAngle, buf, 20, 4);
    dbgdrive.info("angle: %s", buf);
    SerialTerminal::eraseFromCursorEndLine();
    SerialTerminal::moveCursorToNextLine(1);
    ftoa(frontLeftSpeed, buf, 20, 4);
    dbgdrive.info("frontLeftSpeed: %s", buf);
    SerialTerminal::eraseFromCursorEndLine();
    SerialTerminal::moveCursorToNextLine(1);
    ftoa(frontRightSpeed, buf, 20, 4);
    dbgdrive.info("frontRightSpeed: %s", buf);
    SerialTerminal::eraseFromCursorEndLine();
    SerialTerminal::moveCursorToNextLine(1);
    ftoa(centerLeftSpeed, buf, 20, 4);
    dbgdrive.info("centerLeftSpeed: %s", buf);
    SerialTerminal::eraseFromCursorEndLine();
    SerialTerminal::moveCursorToNextLine(1);
    ftoa(centerRightSpeed, buf, 20, 4);
    dbgdrive.info("centerRightSpeed: %s", buf);
    SerialTerminal::eraseFromCursorEndLine();
    SerialTerminal::moveCursorToNextLine(1);
    ftoa(backLeftSpeed, buf, 20, 4);
    dbgdrive.info("backLeftSpeed: %s", buf);
    SerialTerminal::eraseFromCursorEndLine();
    SerialTerminal::moveCursorToNextLine(1);
    ftoa(backRightSpeed, buf, 20, 4);
    dbgdrive.info("backRightSpeed: %s", buf);
    SerialTerminal::eraseFromCursorEndLine();
    SerialTerminal::moveCursorToNextLine(1);

    // SerialTerminal::showCursor();
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

void Drivetrain::waitUntilAvailable()
{
    Timer timer(clock);
    do
    {
        if (!requestUpdate())
        {
            dbgdrive.error("requestUpdate failed!");
            while (1)
                ;
        }
        logTelemetry();
        timer.spinWait(Time::fromSeconds(1.0f));
    } while (!isBusy());
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