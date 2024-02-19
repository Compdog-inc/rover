#include <framework.h>
#include <constants.h>
#include <ioutils.h>
#include <clock.h>
#include <i2c.h>
#include <usart.h>
#include <motor.h>
#include <PWMMotor.h>
#include <pidcontroller.h>
#include <status.h>
#include <serialdebug.h>
#include "commands.h"

DebugInterface debug;

float targetLeftPower = 0.0f;
float targetRightPower = 0.0f;
float leftVelocity = 1.0f;
float rightVelocity = 1.0f;
float turnVelocity = 1.0f;
float currentAngle = 0.0f;

Command command = {};
Command currentCommand = {};

Clock clock;

Motor frontLeftMotor = {9, 8};
Motor frontRightMotor = {11, 10};
Motor centerLeftMotor = {12, 13};
Motor centerRightMotor = {27, 26};
Motor backLeftMotor = {29, 28};
Motor backRightMotor = {31, 30};

PWMMotor frontLeftController = {50};
PWMMotor frontRightController = {50};
PWMMotor centerLeftController = {50};
PWMMotor centerRightController = {50};
PWMMotor backLeftController = {50};
PWMMotor backRightController = {50};

#define WHEEL_PID           \
    {                       \
        &clock, 8.0f, 0.01f \
    }

PIDController frontLeftPID = WHEEL_PID;
PIDController frontRightPID = WHEEL_PID;
PIDController centerLeftPID = WHEEL_PID;
PIDController centerRightPID = WHEEL_PID;
PIDController backLeftPID = WHEEL_PID;
PIDController backRightPID = WHEEL_PID;

bool allWheelsAtTarget()
{
    return frontLeftPID.atTarget(frontLeftController.getSpeed()) &&
           frontRightPID.atTarget(frontRightController.getSpeed()) &&
           centerLeftPID.atTarget(centerLeftController.getSpeed()) &&
           centerRightPID.atTarget(centerRightController.getSpeed()) &&
           backLeftPID.atTarget(backLeftController.getSpeed()) &&
           backRightPID.atTarget(backRightController.getSpeed());
}

// Processes a command and return true if it is complete, false if it needs to be called again
bool processCommand(Command cmd, unsigned long delta)
{
    switch (cmd.id)
    {
    case CMD_DRIVETRAIN_DRIVE:
    {
        // Get target drivetrain power based off direction
        float targetPower = cmd.driveData.direction == DRIVETRAIN_DIRECTION_FORWARD ? 1.0f : -1.0f;

        targetLeftPower = targetPower;
        targetRightPower = targetPower;

        // Command is complete when all wheels have reached their target
        return allWheelsAtTarget();
    }
    case CMD_DRIVETRAIN_STOP:
    {
        targetLeftPower = 0;
        targetRightPower = 0;

        // Command is complete when all wheels have reached their target
        return allWheelsAtTarget();
    }
    case CMD_DRIVETRAIN_TURN:
    {
        // Placeholder without precise angle measurements
        return true;
    }
    case CMD_DRIVETRAIN_SET_VELOCITY:
    {
        leftVelocity = cmd.setVelocityData.leftVelocity;
        rightVelocity = cmd.setVelocityData.rightVelocity;
        return true;
    }
    case CMD_DRIVETRAIN_MOVE:
    {
        // Placeholder without precise distance measurements
        return true;
    }
    }

    return true;
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

#define OK(val) \
    if (!val)   \
        return;

void requestData()
{
    uint8_t buf[4];

    // return the current left and right drivetrain power (current PWM value of motors)
    encodeFloat(buf, 0);
    OK(TWI::write(buf, 4));
    encodeFloat(buf, 0);
    OK(TWI::write(buf, 4));

    // return the current command id
    OK(TWI::write(&currentCommand.id, 1));

    // return the left and right drivetrain velocity (set by master)
    encodeFloat(buf, leftVelocity);
    OK(TWI::write(buf, 4));
    encodeFloat(buf, rightVelocity);
    OK(TWI::write(buf, 4));

    // return the turn velocity (set by master)
    encodeFloat(buf, turnVelocity);
    OK(TWI::write(buf, 4));

    // return the current angle
    encodeFloat(buf, currentAngle);
    OK(TWI::write(buf, 4));
}

void receiveData()
{
    int id = TWI::readByte();
    debug.info("Byte: %i\r\n", id);
    switch (id)
    {
    case CMD_DRIVETRAIN_DRIVE:
    {
        int direction = TWI::readByte();
        debug.info("Dir: %i\r\n", direction);
        command = {};
        command.id = CMD_DRIVETRAIN_DRIVE;
        command.startTime = clock.counter();
        command.driveData.direction = (uint8_t)direction;
        break;
    }
    case CMD_DRIVETRAIN_STOP:
    {
        command = {};
        command.id = CMD_DRIVETRAIN_STOP;
        command.startTime = clock.counter();
        break;
    }
    case CMD_DRIVETRAIN_TURN:
    {
        uint8_t buf[4];
        if (TWI::read(buf, 4) == 4)
        {
            float angle = decodeFloat(buf);
            command = {};
            command.id = CMD_DRIVETRAIN_TURN;
            command.startTime = clock.counter();
            command.turnData = {};
            command.turnData.angle = angle;
        }
        break;
    }
    case CMD_DRIVETRAIN_SET_VELOCITY:
    {
        uint8_t buf[8];
        if (TWI::read(buf, 8) == 8)
        {
            float leftVelocity = decodeFloat(buf);
            float rightVelocity = decodeFloat(buf + 4);
            command = {};
            command.id = CMD_DRIVETRAIN_SET_VELOCITY;
            command.startTime = clock.counter();
            command.setVelocityData = {};
            command.setVelocityData.leftVelocity = leftVelocity;
            command.setVelocityData.rightVelocity = rightVelocity;
        }
        break;
    }
    case CMD_DRIVETRAIN_MOVE:
    {
        uint8_t buf[4];
        if (TWI::read(buf, 4) == 4)
        {
            float distance = decodeFloat(buf);
            command = {};
            command.id = CMD_DRIVETRAIN_MOVE;
            command.startTime = clock.counter();
            command.moveData = {};
            command.moveData.distance = distance;
        }
        break;
    }
    }
}

int main()
{
    debug = DebugInterface("Drivetrain", CURRENT_VERSION);
    debug.printHeader();

    clock.init();
    TWI::enable();
    TWI::connect();
    TWI::disableGeneralCall();
    TWI::setAddress(DRIVETRAIN_I2C);
    TWI::setAddressMask(0x00);
    TWI::setSlave();

    unsigned long prevCommandExec = 0;

    while (1)
    {
        unsigned long time = clock.counter();
        // Execute current command
        if (processCommand(currentCommand, time - prevCommandExec))
        {
            // Check if it has new command queued and set that as current command
            if (command.startTime != currentCommand.startTime)
            {
                currentCommand = command;
            }
            else // Else set current command to None
            {
                currentCommand.id = CMD_NONE;
            }
        }
        prevCommandExec = time;

        // this only works because isDataRequested clears the TWINT flag when false
        if (TWI::isDataRequested())
        {
            debug.info("requested\r\n");
            requestData();
        }
        // if readAvailable returns true, the START event is already consumed by the isDataRequested check
        else if (TWI::readAvailable())
        {
            debug.info("reading\r\n");
            receiveData();
            uint8_t status = TWI::__internal_clearWait();
            debug.info("after read '%s'\n", TWI::nameOfStatus(status));
        }

        _delay_us(1);

        float leftSpeed = targetLeftPower * leftVelocity;
        float rightSpeed = targetRightPower * rightVelocity;

        frontLeftPID.setTarget(leftSpeed);
        centerLeftPID.setTarget(leftSpeed);
        backLeftPID.setTarget(leftSpeed);

        frontRightPID.setTarget(rightSpeed);
        centerRightPID.setTarget(rightSpeed);
        backRightPID.setTarget(rightSpeed);

        frontLeftController.set(frontLeftPID.calculate(frontLeftController.getSpeed()));
        frontRightController.set(frontRightPID.calculate(frontRightController.getSpeed()));
        centerLeftController.set(centerLeftPID.calculate(centerLeftController.getSpeed()));
        centerRightController.set(centerRightPID.calculate(centerRightController.getSpeed()));
        backLeftController.set(backLeftPID.calculate(backLeftController.getSpeed()));
        backRightController.set(backRightPID.calculate(backRightController.getSpeed()));

        float timestamp = clock.seconds();

        frontLeftController.update(frontLeftMotor, timestamp);
        frontRightController.update(frontRightMotor, timestamp);
        centerLeftController.update(centerLeftMotor, timestamp);
        centerRightController.update(centerRightMotor, timestamp);
        backLeftController.update(backLeftMotor, timestamp);
        backRightController.update(backRightMotor, timestamp);

        /*if (currentCommand.id == CMD_NONE && (time - currentCommand.startTime) > 4000000)
        {
            int t = rand() % 7; // stop,forward,backward,left,right
            if (t == 0 || t == 5 || t == 6)
            {
                debug.info("forward\r\n");
                int direction = DRIVETRAIN_DIRECTION_FORWARD;
                command = {};
                command.id = CMD_DRIVETRAIN_DRIVE;
                command.startTime = clock.counter();
                command.driveData.direction = (uint8_t)direction;
                leftVelocity = 1.0f;
                rightVelocity = 1.0f;
            }
            else if (t == 1)
            {
                debug.info("backward\r\n");
                int direction = DRIVETRAIN_DIRECTION_BACKWARD;
                command = {};
                command.id = CMD_DRIVETRAIN_DRIVE;
                command.startTime = clock.counter();
                command.driveData.direction = (uint8_t)direction;
                leftVelocity = 1.0f;
                rightVelocity = 1.0f;
            }
            else if (t == 2)
            {
                debug.info("left\r\n");
                int direction = DRIVETRAIN_DIRECTION_FORWARD;
                command = {};
                command.id = CMD_DRIVETRAIN_DRIVE;
                command.startTime = clock.counter();
                command.driveData.direction = (uint8_t)direction;
                leftVelocity = -1.0f;
                rightVelocity = 1.0f;
            }
            else if (t == 3)
            {
                debug.info("right\r\n");
                int direction = DRIVETRAIN_DIRECTION_FORWARD;
                command = {};
                command.id = CMD_DRIVETRAIN_DRIVE;
                command.startTime = clock.counter();
                command.driveData.direction = (uint8_t)direction;
                leftVelocity = 1.0f;
                rightVelocity = -1.0f;
            }
            else if (t == 4)
            {
                debug.info("stop\r\n");
                command = {};
                command.id = CMD_DRIVETRAIN_STOP;
                command.startTime = clock.counter();
            }
            currentCommand = command;
        }*/
    }
}