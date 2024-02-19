#include "serialterminal.h"
#include "usart.h"

void escape()
{
    uint8_t ch = 27;
    USART::write(&ch, 1);
}

void SerialTerminal::bell()
{
    printf("\a");
}

void SerialTerminal::homeCursor()
{
    escape();
    printf("[H");
}
void SerialTerminal::moveCursor(int line, int column)
{
    escape();
    printf("[{%i};{%i}H", line, column);
    escape();
    printf("[{%i};{%i}f", line, column);
}
void SerialTerminal::moveCursorUp(int num)
{
    escape();
    printf("[%iA", num);
}
void SerialTerminal::moveCursorDown(int num)
{
    escape();
    printf("[%iB", num);
}
void SerialTerminal::moveCursorRight(int num)
{
    escape();
    printf("[%iC", num);
}
void SerialTerminal::moveCursorLeft(int num)
{
    escape();
    printf("[%iD", num);
}
void SerialTerminal::moveCursorToNextLine(int linesDown)
{
    escape();
    printf("[%iE", linesDown);
}
void SerialTerminal::moveCursorToPrevLine(int linesUp)
{
    escape();
    printf("[%iF", linesUp);
}
void SerialTerminal::moveCursorToColumn(int column)
{
    escape();
    printf("[%iG", column);
}
void SerialTerminal::moveCursorLineUp()
{
    escape();
    printf(" M");
}

void SerialTerminal::eraseFromCursorEndScreen()
{
    escape();
    printf("[0J");
}
void SerialTerminal::eraseFromCursorBeginningScreen()
{
    escape();
    printf("[1J");
}
void SerialTerminal::eraseScreen()
{
    escape();
    printf("[2J");
}
void SerialTerminal::eraseFromCursorEndLine()
{
    escape();
    printf("[0K");
}
void SerialTerminal::eraseFromCursorStartLine()
{
    escape();
    printf("[1K");
}
void SerialTerminal::eraseLine()
{
    escape();
    printf("[2K");
}

void SerialTerminal::resetMode()
{
    escape();
    printf("[0m");
}
void SerialTerminal::boldMode(bool enabled)
{
    escape();
    if (enabled)
        printf("[1m");
    else
        printf("[22m");
}
void SerialTerminal::dimMode(bool enabled)
{
    escape();
    if (enabled)
        printf("[2m");
    else
        printf("[22m");
}
void SerialTerminal::italicMode(bool enabled)
{
    escape();
    if (enabled)
        printf("[3m");
    else
        printf("[23m");
}
void SerialTerminal::underlineMode(bool enabled)
{
    escape();
    if (enabled)
        printf("[4m");
    else
        printf("[24m");
}
void SerialTerminal::doubleUnderlineMode(bool enabled)
{
    escape();
    if (enabled)
        printf("[21m");
    else
        printf("[24m");
}
void SerialTerminal::blinkingMode(bool enabled)
{
    escape();
    if (enabled)
        printf("[5m");
    else
        printf("[25m");
}
void SerialTerminal::inverseMode(bool enabled)
{
    escape();
    if (enabled)
        printf("[7m");
    else
        printf("[27m");
}
void SerialTerminal::invisibleMode(bool enabled)
{
    escape();
    if (enabled)
        printf("[8m");
    else
        printf("[28m");
}
void SerialTerminal::strikethroughMode(bool enabled)
{
    escape();
    if (enabled)
        printf("[9m");
    else
        printf("[29m");
}

void SerialTerminal::setForegroundColor(TerminalColor color) {}
void SerialTerminal::setBackgroundColor(TerminalColor color) {}
void SerialTerminal::setForegroundColor(uint8_t color256) {}
void SerialTerminal::setBackgroundColor(uint8_t color256) {}
void SerialTerminal::setForegroundColor(uint8_t r, uint8_t g, uint8_t b) {}
void SerialTerminal::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {}

void SerialTerminal::hideCursor() {}
void SerialTerminal::showCursor() {}