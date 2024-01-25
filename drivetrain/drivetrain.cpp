#include <framework.h>
#include <constants.h>
#include <ioutils.h>
#include <clock.h>
#include <twi.h>

typedef struct
{
    uint8_t direction;
} DriveCommandData;

typedef struct
{
    float angle;
} TurnCommandData;

typedef struct
{
    float leftVelocity;
    float rightVelocity;
} SetVelocityCommandData;

typedef struct
{
    float distance;
} MoveCommandData;

typedef struct
{
    uint8_t id;
    unsigned long startTime;

    union
    {
        DriveCommandData driveData;
        TurnCommandData turnData;
        SetVelocityCommandData setVelocityData;
        MoveCommandData moveData;
    };
} Command;

float currentLeftPower = 0.0f;
float currentRightPower = 0.0f;
float leftVelocity = 1.0f;
float rightVelocity = 1.0f;
float turnVelocity = 1.0f;
float currentAngle = 0.0f;

Command command = {};
Command currentCommand = {};

Clock clock;

int main()
{
    clock.init();
    TWI::enable();
    // Wire.begin(DRIVETRAIN_I2C);
    // Wire.onRequest(requestData);
    // Wire.onReceive(receiveData);
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
        pidWait();
    }
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

        // Update current power values using pid
        currentLeftPower = pidPowerTarget(currentLeftPower, targetPower, delta);
        currentRightPower = pidPowerTarget(currentRightPower, targetPower, delta);

        // Check if current power values are near the target
        bool leftThresh = pidThreshold(currentLeftPower, targetPower);
        bool rightThresh = pidThreshold(currentRightPower, targetPower);

        // Snap values to the target if they are near
        if (leftThresh)
            currentLeftPower = targetPower;
        if (rightThresh)
            currentRightPower = targetPower;

        // Command is complete when both left and right power have reached the target
        return leftThresh && rightThresh;
    }
    case CMD_DRIVETRAIN_STOP:
    {
        // Update current power values using pid
        currentLeftPower = pidPowerTarget(currentLeftPower, 0, delta);
        currentRightPower = pidPowerTarget(currentRightPower, 0, delta);

        // Check if current power values are near the target
        bool leftThresh = pidThreshold(currentLeftPower, 0);
        bool rightThresh = pidThreshold(currentRightPower, 0);

        // Set power to exactly 0 to avoid slow drifting
        if (leftThresh)
            currentLeftPower = 0;
        if (rightThresh)
            currentRightPower = 0;

        // Command is complete when both left and right power have reached the target
        return leftThresh && rightThresh;
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

float pidPowerTarget(float current, float target, unsigned long delta)
{
}

bool pidThreshold(float current, float target)
{
    return current >= target - 0.001f && current <= target + 0.001f;
}

void pidWait()
{
    _delay_us(20);
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

void requestData()
{
    uint8_t buf[4];

    // return the current left and right drivetrain power (current PWM value of motors)
    encodeFloat(buf, currentLeftPower);
    Wire.write(buf, 4);
    encodeFloat(buf, currentRightPower);
    Wire.write(buf, 4);

    // return the current command id
    Wire.write(currentCommand.id);

    // return the left and right drivetrain velocity (set by master)
    encodeFloat(buf, leftVelocity);
    Wire.write(buf, 4);
    encodeFloat(buf, rightVelocity);
    Wire.write(buf, 4);

    // return the turn velocity (set by master)
    encodeFloat(buf, turnVelocity);
    Wire.write(buf, 4);

    // return the current angle
    encodeFloat(buf, currentAngle);
    Wire.write(buf, 4);
}

void receiveData(int numBytes)
{
    if (numBytes >= 1)
    {
        int id = Wire.read();
        numBytes--;
        switch (id)
        {
        case CMD_DRIVETRAIN_DRIVE:
        {
            if (numBytes >= 1)
            {
                int direction = Wire.read();
                command = {};
                command.id = CMD_DRIVETRAIN_DRIVE;
                command.startTime = clock.counter();
                command.driveData.direction = (uint8_t)direction;
            }
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
            if (numBytes >= 4)
            {
                uint8_t buf[4];
                if (Wire.readBytes(buf, 4) == 4)
                {
                    float angle = decodeFloat(buf);
                    command = {};
                    command.id = CMD_DRIVETRAIN_TURN;
                    command.startTime = clock.counter();
                    command.turnData = {};
                    command.turnData.angle = angle;
                }
            }
            break;
        }
        case CMD_DRIVETRAIN_SET_VELOCITY:
        {
            if (numBytes >= 8)
            {
                uint8_t buf[8];
                if (Wire.readBytes(buf, 8) == 8)
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
            }
            break;
        }
        case CMD_DRIVETRAIN_MOVE:
        {
            if (numBytes >= 4)
            {
                uint8_t buf[4];
                if (Wire.readBytes(buf, 4) == 4)
                {
                    float distance = decodeFloat(buf);
                    command = {};
                    command.id = CMD_DRIVETRAIN_MOVE;
                    command.startTime = clock.counter();
                    command.moveData = {};
                    command.moveData.distance = distance;
                }
            }
            break;
        }
        }
    }
}