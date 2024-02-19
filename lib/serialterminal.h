#ifndef SERIAL_TERMINAL_H
#define SERIAL_TERMINAL_H

#include "framework.h"

enum class TerminalColor
{
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    Default,
    BrightBlack,
    BrightRed,
    BrightGreen,
    BrightYellow,
    BrightBlue,
    BrightMagenta,
    BrightCyan,
    BrightWhite
};

typedef struct SerialTerminal
{
public:
    void bell();

    void homeCursor();
    void moveCursor(int line, int column);
    void moveCursorUp(int num);
    void moveCursorDown(int num);
    void moveCursorRight(int num);
    void moveCursorLeft(int num);
    void moveCursorToNextLine(int linesDown);
    void moveCursorToPrevLine(int linesUp);
    void moveCursorToColumn(int column);
    void moveCursorLineUp();

    void eraseFromCursorEndScreen();
    void eraseFromCursorBeginningScreen();
    void eraseScreen();
    void eraseFromCursorEndLine();
    void eraseFromCursorStartLine();
    void eraseLine();

    void resetMode();
    void boldMode(bool enabled);
    void dimMode(bool enabled);
    void italicMode(bool enabled);
    void underlineMode(bool enabled);
    void doubleUnderlineMode(bool enabled);
    void blinkingMode(bool enabled);
    void inverseMode(bool enabled);
    void invisibleMode(bool enabled);
    void strikethroughMode(bool enabled);

    void setForegroundColor(TerminalColor color);
    void setBackgroundColor(TerminalColor color);
    void setForegroundColor(uint8_t color256);
    void setBackgroundColor(uint8_t color256);
    void setForegroundColor(uint8_t r, uint8_t g, uint8_t b);
    void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);

    void hideCursor();
    void showCursor();

private:
} SerialTerminal;

#endif