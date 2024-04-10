#ifndef RADIO_H
#define RADIO_H

#include "framework.h"
#include "ioutils.h"

enum class RadioMode
{
    FastPowerSaving,
    SlowPowerSaving,
    UltraLongDistance,
    Normal
};

enum class RadioParity
{
    None,
    Even,
    Odd
};

#define RADIO_POWER_NEG_1 1
#define RADIO_POWER_2 2
#define RADIO_POWER_5 3
#define RADIO_POWER_8 4
#define RADIO_POWER_11 5
#define RADIO_POWER_14 6
#define RADIO_POWER_17 7
#define RADIO_POWER_20 8

#define RADIO_STOPBIT_0 0
#define RADIO_STOPBIT_1 1
#define RADIO_STOPBIT_2 2
#define RADIO_STOPBIT_1_5 3

typedef struct Radio
{
public:
    Radio(IOPort *io);

    void disable();
    void enable();
    void enterSetup();
    void exitSetup();

    void performTest();
    void setBaud(long baud);
    void setChannel(uint8_t channel);
    void setMode(RadioMode mode);
    void setPower(uint8_t power);
    void setUart(uint8_t dataBits, RadioParity parity, uint8_t stopBits);

    void getBaud();
    void getChannel();
    void getMode();
    void getPower();
    void getVersion();

    void sleep();

    void reset();

    void send(uint8_t data);
    void send(uint8_t *data, int offset, int count);
    void request(uint8_t *buf, int offset, int count);

private:
    IOPort *io;
} Radio;

#endif