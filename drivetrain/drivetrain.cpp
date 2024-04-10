#include <framework.h>
#include <string.h>
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
#include <serialize.h>
#include <staticqueue.h>

DebugInterface debug;
ByteStream i2c;

float targetLeftPower = 0.0f;
float targetRightPower = 0.0f;
float leftVelocity = 1.0f;
float rightVelocity = 1.0f;
float turnVelocity = 1.0f;
float currentAngle = 0.0f;

static StaticQueue<Command *> command_queue(COMMAND_QUEUE_SIZE);

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

void setPIDTargets()
{
    float leftSpeed = targetLeftPower * leftVelocity;
    float rightSpeed = targetRightPower * rightVelocity;

    frontLeftPID.setTarget(leftSpeed);
    centerLeftPID.setTarget(leftSpeed);
    backLeftPID.setTarget(leftSpeed);

    frontRightPID.setTarget(rightSpeed);
    centerRightPID.setTarget(rightSpeed);
    backRightPID.setTarget(rightSpeed);
}

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
bool processCommand(Command *cmd, unsigned long delta)
{
    if (cmd == nullptr)
        return true;
    switch (cmd->id)
    {
    case CMD_DRIVETRAIN_DRIVE:
    {
        // Get target drivetrain power based off direction
        float targetPower = cmd->driveData.direction == DRIVETRAIN_DIRECTION_FORWARD ? 1.0f : -1.0f;

        targetLeftPower = targetPower;
        targetRightPower = targetPower;
        setPIDTargets();

        // Command is complete when all wheels have reached their target
        return allWheelsAtTarget();
    }
    case CMD_DRIVETRAIN_STOP:
    {
        targetLeftPower = 0;
        targetRightPower = 0;
        setPIDTargets();

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
        leftVelocity = cmd->setVelocityData.leftVelocity;
        rightVelocity = cmd->setVelocityData.rightVelocity;
        setPIDTargets();
        return allWheelsAtTarget();
    }
    case CMD_DRIVETRAIN_SET_TURN_VELOCITY:
    {
        turnVelocity = cmd->setTurnVelocityData.velocity;
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

Status requestData()
{
    uint8_t buf[4];

    // return the current motor speeds
    encodeFloat(buf, frontLeftController.getSpeed());
    if (i2c.write(buf, 0, 4) != 4)
        return Status::INCOMPLETE_DATA;
    encodeFloat(buf, frontRightController.getSpeed());
    if (i2c.write(buf, 0, 4) != 4)
        return Status::INCOMPLETE_DATA;
    encodeFloat(buf, centerLeftController.getSpeed());
    if (i2c.write(buf, 0, 4) != 4)
        return Status::INCOMPLETE_DATA;
    encodeFloat(buf, centerRightController.getSpeed());
    if (i2c.write(buf, 0, 4) != 4)
        return Status::INCOMPLETE_DATA;
    encodeFloat(buf, backLeftController.getSpeed());
    if (i2c.write(buf, 0, 4) != 4)
        return Status::INCOMPLETE_DATA;
    encodeFloat(buf, backRightController.getSpeed());
    if (i2c.write(buf, 0, 4) != 4)
        return Status::INCOMPLETE_DATA;

    // return the current command id
    uint8_t id = -1;
    if (i2c.write(&id, 0, 1) != 1)
        return Status::INCOMPLETE_DATA;

    // return the left and right drivetrain power
    encodeFloat(buf, targetLeftPower);
    if (i2c.write(buf, 0, 4) != 4)
        return Status::INCOMPLETE_DATA;
    encodeFloat(buf, targetRightPower);
    if (i2c.write(buf, 0, 4) != 4)
        return Status::INCOMPLETE_DATA;

    // return the current angle
    encodeFloat(buf, currentAngle);
    if (i2c.write(buf, 0, 4) != 4)
        return Status::INCOMPLETE_DATA;

    return Status::OK;
}

Status receiveData()
{
    Command *command = (Command *)malloc(sizeof(Command));
    memset(command, 0, sizeof(Command));

    int id = i2c.read();

    switch (id)
    {
    case CMD_DRIVETRAIN_DRIVE:
    {
        int direction = i2c.read();
        command->id = CMD_DRIVETRAIN_DRIVE;
        command->startTime = clock.counter();
        command->driveData.direction = (uint8_t)direction;
        break;
    }
    case CMD_DRIVETRAIN_STOP:
    {
        command->id = CMD_DRIVETRAIN_STOP;
        command->startTime = clock.counter();
        break;
    }
    case CMD_DRIVETRAIN_TURN:
    {
        uint8_t buf[4];
        if (i2c.read(buf, 0, 4) == 4)
        {
            float angle = decodeFloat(buf);
            command->id = CMD_DRIVETRAIN_TURN;
            command->startTime = clock.counter();
            command->turnData = {};
            command->turnData.angle = angle;
        }
        break;
    }
    case CMD_DRIVETRAIN_SET_VELOCITY:
    {
        uint8_t buf[8];
        if (i2c.read(buf, 0, 8) == 8)
        {
            float leftVelocity = decodeFloat(buf);
            float rightVelocity = decodeFloat(buf + 4);
            command->id = CMD_DRIVETRAIN_SET_VELOCITY;
            command->startTime = clock.counter();
            command->setVelocityData = {};
            command->setVelocityData.leftVelocity = leftVelocity;
            command->setVelocityData.rightVelocity = rightVelocity;
        }
        break;
    }
    case CMD_DRIVETRAIN_SET_TURN_VELOCITY:
    {
        uint8_t buf[4];
        if (i2c.read(buf, 0, 4) == 4)
        {
            float velocity = decodeFloat(buf);
            command->id = CMD_DRIVETRAIN_SET_TURN_VELOCITY;
            command->startTime = clock.counter();
            command->setTurnVelocityData = {};
            command->setTurnVelocityData.velocity = velocity;
        }
        break;
    }
    case CMD_DRIVETRAIN_MOVE:
    {
        uint8_t buf[4];
        if (i2c.read(buf, 0, 4) == 4)
        {
            float distance = decodeFloat(buf);
            command->id = CMD_DRIVETRAIN_MOVE;
            command->startTime = clock.counter();
            command->moveData = {};
            command->moveData.distance = distance;
        }
        break;
    }
    default:
    {
        free(command);
        return Status::UNKOWN_ID;
    }
    }

    bool ok = command_queue.Enqueue(command);
    if (!ok)
    {
        free(command);
        return Status::CORRUPTED;
    }

    return Status::OK;
}

int main()
{
    debug = DebugInterface("Drivetrain", CURRENT_VERSION);
    debug.printHeader();

    clock.init();
    TWI::enable(DRIVETRAIN_I2C);
    i2c = TWI::getStream();

    unsigned long prevCommandExec = 0;

    Command *currentCommand = nullptr;

    while (1)
    {
        unsigned long time = clock.counter();
        // Execute current command
        if (processCommand(currentCommand, time - prevCommandExec))
        {
            free(currentCommand);
            currentCommand = command_queue.Dequeue();
        }
        prevCommandExec = time;

        if (TWI::isDataRequested())
        {
            Status status = requestData();
            if (status != Status::OK)
            {
                debug.error("requestData returned '%s'\n", nameOfStatus(status));
            }
        }
        else if (i2c.length() > 0)
        {
            Status status = receiveData();
            if (status != Status::OK)
            {
                debug.error("receiveData returned '%s'\n", nameOfStatus(status));
            }
        }

        _delay_us(1);

        setPIDTargets();

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
    }
}