#ifndef SERIAL_DEBUG_H
#define SERIAL_DEBUG_H

#include "framework.h"

typedef struct Version
{
    Version(uint32_t version);
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
    uint8_t build;
} Version;

typedef struct DebugInterface
{
public:
    DebugInterface();
    DebugInterface(const char *name, Version version);
    ~DebugInterface();
    void printHeader();
    void info(const char *__fmt, ...);
    void warn(const char *__fmt, ...);
    void error(const char *__fmt, ...);

private:
    const char *name;
    Version version;
} DebugInterface;

#include "../include/version.h"

#define CURRENT_VERSION \
    ((Version){CURRENT_BUILD})

#endif