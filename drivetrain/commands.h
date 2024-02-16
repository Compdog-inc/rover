#ifndef DRIVETRAIN_COMMANDS_H
#define DRIVETRAIN_COMMANDS_H

#include <framework.h>

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

#endif