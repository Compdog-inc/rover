#include "framework.h"
#include "serialdebug.h"
#include "usart.h"
#include <string.h>
#include <stdlib.h>

Version::Version(uint32_t version)
{
    major = (uint8_t)((version >> 24) & 0xFF);
    minor = (uint8_t)((version >> 16) & 0xFF);
    patch = (uint8_t)((version >> 8) & 0xFF);
    build = (uint8_t)(version & 0xFF);
}

DebugInterface::DebugInterface(const char *name, Version _version) : version(_version)
{
    USART::enable();
    USART::setBaudRate(115200);
    USART::redirectStdout();

    this->name = name;
}

DebugInterface::DebugInterface() : version(0)
{
    this->name = nullptr;
}

DebugInterface::~DebugInterface()
{
}

void DebugInterface::printHeader()
{
    printf("------- SerialDebug Interface --------\r\n    %s, v%u.%u.%u.%u\r\n--------------------------------------\r\n", name, version.major, version.minor, version.patch, version.build);
}

void DebugInterface::info(const char *__fmt, ...)
{
    va_list args;

    va_start(args, __fmt);

    printf("[INFO/%s]: ", name);
    vprintf(__fmt, args);

    va_end(args);
}

void DebugInterface::warn(const char *__fmt, ...)
{
    va_list args;

    va_start(args, __fmt);

    printf("[WARN/%s]: ", name);
    vprintf(__fmt, args);

    va_end(args);
}

void DebugInterface::error(const char *__fmt, ...)
{
    va_list args;

    va_start(args, __fmt);

    printf("[ERROR/%s]: ", name);
    vprintf(__fmt, args);

    va_end(args);
}

void DebugInterface::array(uint8_t *buf, int size)
{
    printf("[TRACE/%s]: [", name);
    for (int i = 0; i < size; i++)
    {
        printf("%u", buf[i]);
        if (i < size - 1)
            putchar(',');
    }
    printf("]\n");
}