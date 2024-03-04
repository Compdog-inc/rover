#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define DRIVETRAIN_I2C 0xA4
#define ENVIRONMENT_I2C 0xA6

#define CMD_NONE 0x00

#define CMD_DRIVETRAIN_DRIVE 0x01
#define CMD_DRIVETRAIN_STOP 0x02
#define CMD_DRIVETRAIN_TURN 0x03
#define CMD_DRIVETRAIN_SET_VELOCITY 0x04
#define CMD_DRIVETRAIN_SET_TURN_VELOCITY 0x05
#define CMD_DRIVETRAIN_MOVE 0x06

#define DRIVETRAIN_DIRECTION_FORWARD 1
#define DRIVETRAIN_DIRECTION_BACKWARD 2

enum class Status
{
    OK,
    UNKOWN_ID,
    INCOMPLETE_DATA,
    INVALID_FORMAT
};

const char *nameOfStatus(Status status)
{
    switch (status)
    {
    case Status::OK:
        return "OK";
    case Status::INCOMPLETE_DATA:
        return "INCOMPLETE_DATA";
    case Status::INVALID_FORMAT:
        return "INVALID_FORMAT";
    case Status::UNKOWN_ID:
        return "UNKNOWN_ID";
    default:
        return "UNKNOWN";
    }
}

#endif