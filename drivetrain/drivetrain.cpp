#include <framework.h>
#include <constants.h>
#include <ioutils.h>
#include <clock.h>
#include <i2c.h>
#include <usart.h>
#include <motor.h>
#include <status.h>

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
float rightVelocity = -1.0f;
float turnVelocity = 1.0f;
float currentAngle = 0.0f;

Command command = {};
Command currentCommand = {};

Clock clock;

Motor motor0 = {9, 8};
Motor motor1 = {11, 10};
Motor motor2 = {12, 13};
Motor motor3 = {27, 26};
Motor motor4 = {29, 28};
Motor motor5 = {31, 30};

float pidPowerTarget(float current, float target, unsigned long delta)
{
    float deltaUs = clock.toMicros(delta);
    float deltaS = deltaUs / 1000000.0f;
    return current + fmin(1.0f, fmax(-1.0f, (target - current))) * deltaS * 4.0f;
}

bool pidThreshold(float current, float target)
{
    return current >= target - 0.01f && current <= target + 0.01f;
}

void pidWait()
{
    _delay_us(1);
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
    encodeFloat(buf, currentLeftPower);
    OK(TWI::write(buf, 4));
    encodeFloat(buf, currentRightPower);
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
    printf("Byte: %i\n", id);
    switch (id)
    {
    case CMD_DRIVETRAIN_DRIVE:
    {
        int direction = TWI::readByte();
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
    USART::enable();
    USART::setBaudRate(115200);
    USART::redirectStdout();

    printf("init\n");

    clock.init();
    TWI::enable();
    TWI::connect();
    TWI::disableGeneralCall();
    TWI::setAddress(DRIVETRAIN_I2C);
    // TWI::setAddressMask(0x00);
    TWI::setSlave();

    currentCommand.id = CMD_NONE;

    unsigned long prevCommandExec = 0;

    printf("poweron reset ");
    if (poweron_reset())
    {
        printf("yes\n");
    }
    else
    {
        printf("no\n");
    }

    printf("external reset ");
    if (external_reset())
    {
        printf("yes\n");
    }
    else
    {
        printf("no\n");
    }

    printf("brownout reset ");
    if (brownout_reset())
    {
        printf("yes\n");
    }
    else
    {
        printf("no\n");
    }

    if (brownout_reset() || watchdog_reset() || external_reset() || poweron_reset())
    {
        while (1)
            ;
    }

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
        if (TWI::isDataRequested())
        {
            printf("requested\n");
            requestData();
        }
        else if (TWI::readAvailable())
        {
            printf("reading\n");
            receiveData();
        }

        pidWait();

        float leftSpeed = currentLeftPower * leftVelocity;
        float rightSpeed = currentRightPower * rightVelocity;
        uint16_t leftPWM = (uint16_t)floor(fmin(1.0f, fabs(leftSpeed)) * (float)UINT16_MAX);
        uint16_t rightPWM = (uint16_t)floor(fmin(1.0f, fabs(leftSpeed)) * (float)UINT16_MAX);

        motor0.setPWM(leftPWM);
        motor2.setPWM(leftPWM);
        motor4.setPWM(leftPWM);

        motor1.setPWM(rightPWM);
        motor3.setPWM(rightPWM);
        motor5.setPWM(rightPWM);

        float timestamp = clock.micros() / 1000000.0f;
        if (motor0.inPWMHigh(timestamp))
        {
            if (leftSpeed > 0)
                motor0.clockwise();
            else
                motor0.counterclockwise();
        }
        else
            motor0.stop();
        if (motor1.inPWMHigh(timestamp))
        {
            if (rightSpeed > 0)
                motor1.clockwise();
            else
                motor1.counterclockwise();
        }
        else
            motor1.stop();
        if (motor2.inPWMHigh(timestamp))
        {
            if (leftSpeed > 0)
                motor2.clockwise();
            else
                motor2.counterclockwise();
        }
        else
            motor2.stop();
        if (motor3.inPWMHigh(timestamp))
        {
            if (rightSpeed > 0)
                motor3.clockwise();
            else
                motor3.counterclockwise();
        }
        else
            motor3.stop();
        if (motor4.inPWMHigh(timestamp))
        {
            if (leftSpeed > 0)
                motor4.clockwise();
            else
                motor4.counterclockwise();
        }
        else
            motor4.stop();
        if (motor5.inPWMHigh(timestamp))
        {
            if (rightSpeed > 0)
                motor5.clockwise();
            else
                motor5.counterclockwise();
        }
        else
            motor5.stop();

        if (currentCommand.id == CMD_NONE && (time - currentCommand.startTime) > 4000000)
        {
            int t = rand() % 7; // stop,forward,backward,left,right
            if (t == 0 || t == 5 || t == 6)
            {
                printf("forward\n");
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
                printf("backward\n");
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
                printf("left\n");
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
                printf("right\n");
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
                printf("stop\n");
                command = {};
                command.id = CMD_DRIVETRAIN_STOP;
                command.startTime = clock.counter();
            }
            currentCommand = command;
        }
    }
}