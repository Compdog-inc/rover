#include "framework.h"
#include "serialdebug.h"
#include "usart.h"
#include <string.h>
#include <stdlib.h>
#include "serialterminal.h"

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
    SerialTerminal::resetMode();
    SerialTerminal::setForegroundColor(TerminalColor::BrightCyan);
    printf_P(PSTR("------- SerialDebug Interface --------\r\n    %s, v%u.%u.%u.%u\r\n--------------------------------------\r\n"), name, version.major, version.minor, version.patch, version.build);
    SerialTerminal::setForegroundColor(TerminalColor::Default);
}

void DebugInterface::info_P(const char *__fmt, ...)
{
    va_list args;

    va_start(args, __fmt);

    SerialTerminal::setForegroundColor(TerminalColor::White);
    SerialTerminal::boldMode(true);
    printf_P(PSTR("[INFO/%s]: "), name);
    SerialTerminal::boldMode(false);
    vfprintf_P(stdout, __fmt, args);
    SerialTerminal::setForegroundColor(TerminalColor::Default);

    va_end(args);
}

void DebugInterface::info(const char *__fmt, ...)
{
    va_list args;

    va_start(args, __fmt);

    SerialTerminal::setForegroundColor(TerminalColor::White);
    SerialTerminal::boldMode(true);
    printf_P(PSTR("[INFO/%s]: "), name);
    SerialTerminal::boldMode(false);
    vfprintf(stdout, __fmt, args);
    SerialTerminal::setForegroundColor(TerminalColor::Default);

    va_end(args);
}

void DebugInterface::warn(const char *__fmt, ...)
{
    va_list args;

    va_start(args, __fmt);

    SerialTerminal::setForegroundColor(TerminalColor::Yellow);
    SerialTerminal::boldMode(true);
    printf_P(PSTR("[WARN/%s]: "), name);
    SerialTerminal::boldMode(false);
    vfprintf(stdout, __fmt, args);
    SerialTerminal::setForegroundColor(TerminalColor::Default);

    va_end(args);
}

void DebugInterface::warn_P(const char *__fmt, ...)
{
    va_list args;

    va_start(args, __fmt);

    SerialTerminal::setForegroundColor(TerminalColor::Yellow);
    SerialTerminal::boldMode(true);
    printf_P(PSTR("[WARN/%s]: "), name);
    SerialTerminal::boldMode(false);
    vfprintf_P(stdout, __fmt, args);
    SerialTerminal::setForegroundColor(TerminalColor::Default);

    va_end(args);
}

void DebugInterface::error(const char *__fmt, ...)
{
    va_list args;

    va_start(args, __fmt);

    SerialTerminal::setForegroundColor(TerminalColor::Red);
    SerialTerminal::boldMode(true);
    printf_P(PSTR("[ERROR/%s]: "), name);
    SerialTerminal::boldMode(false);
    vfprintf(stdout, __fmt, args);
    SerialTerminal::setForegroundColor(TerminalColor::Default);

    va_end(args);
}

void DebugInterface::error_P(const char *__fmt, ...)
{
    va_list args;

    va_start(args, __fmt);

    SerialTerminal::setForegroundColor(TerminalColor::Red);
    SerialTerminal::boldMode(true);
    printf_P(PSTR("[ERROR/%s]: "), name);
    SerialTerminal::boldMode(false);
    vfprintf_P(stdout, __fmt, args);
    SerialTerminal::setForegroundColor(TerminalColor::Default);

    va_end(args);
}

void DebugInterface::array(uint8_t *buf, int size)
{
    SerialTerminal::setForegroundColor(TerminalColor::Magenta);
    SerialTerminal::boldMode(true);
    printf_P(PSTR("[TRACE/%s]:"), name);
    SerialTerminal::boldMode(false);
    printf_P(PSTR(" ["));
    for (int i = 0; i < size; i++)
    {
        printf_P(PSTR("%u"), buf[i]);
        if (i < size - 1)
            putchar(',');
    }
    printf_P(PSTR("]\n"));
    SerialTerminal::setForegroundColor(TerminalColor::Default);
}