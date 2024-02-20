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
    printf("[%i;%iH", line, column);
    escape();
    printf("[%i;%if", line, column);
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

void SerialTerminal::setForegroundColor(TerminalColor color)
{
    escape();
    switch (color)
    {
    case TerminalColor::Black:
        printf("[30m");
        break;
    case TerminalColor::Red:
        printf("[31m");
        break;
    case TerminalColor::Green:
        printf("[32m");
        break;
    case TerminalColor::Yellow:
        printf("[33m");
        break;
    case TerminalColor::Blue:
        printf("[34m");
        break;
    case TerminalColor::Magenta:
        printf("[35m");
        break;
    case TerminalColor::Cyan:
        printf("[36m");
        break;
    case TerminalColor::White:
        printf("[37m");
        break;
    case TerminalColor::Default:
        printf("[39m");
        break;
    case TerminalColor::BrightBlack:
        printf("[90m");
        break;
    case TerminalColor::BrightRed:
        printf("[91m");
        break;
    case TerminalColor::BrightGreen:
        printf("[92m");
        break;
    case TerminalColor::BrightYellow:
        printf("[93m");
        break;
    case TerminalColor::BrightBlue:
        printf("[94m");
        break;
    case TerminalColor::BrightMagenta:
        printf("[95m");
        break;
    case TerminalColor::BrightCyan:
        printf("[96m");
        break;
    case TerminalColor::BrightWhite:
        printf("[97m");
        break;
    }
}
void SerialTerminal::setBackgroundColor(TerminalColor color)
{
    escape();
    switch (color)
    {
    case TerminalColor::Black:
        printf("[40m");
        break;
    case TerminalColor::Red:
        printf("[41m");
        break;
    case TerminalColor::Green:
        printf("[42m");
        break;
    case TerminalColor::Yellow:
        printf("[43m");
        break;
    case TerminalColor::Blue:
        printf("[44m");
        break;
    case TerminalColor::Magenta:
        printf("[45m");
        break;
    case TerminalColor::Cyan:
        printf("[46m");
        break;
    case TerminalColor::White:
        printf("[47m");
        break;
    case TerminalColor::Default:
        printf("[49m");
        break;
    case TerminalColor::BrightBlack:
        printf("[100m");
        break;
    case TerminalColor::BrightRed:
        printf("[101m");
        break;
    case TerminalColor::BrightGreen:
        printf("[102m");
        break;
    case TerminalColor::BrightYellow:
        printf("[103m");
        break;
    case TerminalColor::BrightBlue:
        printf("[104m");
        break;
    case TerminalColor::BrightMagenta:
        printf("[105m");
        break;
    case TerminalColor::BrightCyan:
        printf("[106m");
        break;
    case TerminalColor::BrightWhite:
        printf("[107m");
        break;
    }
}
void SerialTerminal::setForegroundColor(uint8_t color256)
{
    escape();
    printf("[38;5;%um", color256);
}
void SerialTerminal::setBackgroundColor(uint8_t color256)
{
    escape();
    printf("[48;5;%um", color256);
}
void SerialTerminal::setForegroundColor(uint8_t r, uint8_t g, uint8_t b)
{
    escape();
    printf("[38;2;%u;%u;%um", r, g, b);
}
void SerialTerminal::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b)
{
    escape();
    printf("[48;2;%u;%u;%um", r, g, b);
}

void SerialTerminal::hideCursor()
{
    escape();
    printf("[?25l");
}
void SerialTerminal::showCursor()
{
    escape();
    printf("[?25h");
}