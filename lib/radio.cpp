#include "framework.h"
#include "radio.h"

#define PU_BAUD_RATE 9600L

#include "internal/picoUART/picoUART.h"
#include <avr/pgmspace.h>

#define RADIO_PWR_PIN _D6
#define RADIO_SET_PIN _D7

#define MAX_LINE_LENGTH 16
char line[MAX_LINE_LENGTH];

Radio::Radio(IOPort *io)
{
    this->io = io;
    io->reset(RADIO_PWR_PIN);
    io->reset(RADIO_SET_PIN);
    io->set_dir(RADIO_PWR_PIN, IODir::Out);
    io->set_dir(RADIO_SET_PIN, IODir::Out);
    io->put(RADIO_PWR_PIN, false);
    io->put(RADIO_SET_PIN, true);

    io->set_dir(_B1, IODir::In);
    io->set_dir(_B0, IODir::In);
    io->set_pull_up(_B0, true);
    io->set_pull_up(_B1, true);
}

void Radio::disable()
{
    io->put(RADIO_PWR_PIN, false);
}

void Radio::enable()
{
    io->put(RADIO_PWR_PIN, true);
}

void Radio::enterSetup()
{
    io->put(RADIO_SET_PIN, false);
}

void Radio::exitSetup()
{
    io->put(RADIO_SET_PIN, true);
}

uint8_t rxLine()
{
    uint8_t i = 0;
    char c;
    do
    {
        c = pu_rx();
        line[i++] = c;
    } while (c != '\n' && i < MAX_LINE_LENGTH);
    return i;
}

void echoLine()
{
    uint8_t len = rxLine();
    for (uint8_t i = 0; i < len; i++)
    {
        putchar(line[i]);
    }
}

void Radio::performTest()
{
    prints_P(PSTR("AT"));
    echoLine();
}

void Radio::setBaud(long baud)
{
    int len = sprintf_P(line, PSTR("AT+B%i"), baud);
    for (int i = 0; i < len; i++)
    {
        pu_tx(line[i]);
    }
    echoLine();
}

void Radio::setChannel(uint8_t channel)
{
    int len = sprintf_P(line, PSTR("AT+C%03u"), channel);
    for (int i = 0; i < len; i++)
    {
        pu_tx(line[i]);
    }
    echoLine();
}

void Radio::setMode(RadioMode mode)
{
    switch (mode)
    {
    case RadioMode::FastPowerSaving:
        prints_P(PSTR("AT+FU1"));
        break;
    case RadioMode::SlowPowerSaving:
        prints_P(PSTR("AT+FU2"));
        break;
    case RadioMode::Normal:
        prints_P(PSTR("AT+FU3"));
        break;
    case RadioMode::UltraLongDistance:
        prints_P(PSTR("AT+FU4"));
        break;
    }
    echoLine();
}

void Radio::setPower(uint8_t power)
{
    int len = sprintf_P(line, PSTR("AT+P%u"), power);
    for (int i = 0; i < len; i++)
    {
        pu_tx(line[i]);
    }
    echoLine();
}

void Radio::setUart(uint8_t dataBits, RadioParity parity, uint8_t stopBits)
{
    int len;
    switch (parity)
    {
    case RadioParity::None:
        len = sprintf_P(line, PSTR("AT+U%uN%u"), dataBits, stopBits);
        break;
    case RadioParity::Even:
        len = sprintf_P(line, PSTR("AT+U%uE%u"), dataBits, stopBits);
        break;
    case RadioParity::Odd:
        len = sprintf_P(line, PSTR("AT+U%uO%u"), dataBits, stopBits);
        break;
    default:
        return;
    }

    for (int i = 0; i < len; i++)
    {
        pu_tx(line[i]);
    }
    echoLine();
}

void Radio::getBaud() {}
void Radio::getChannel() {}
void Radio::getMode() {}
void Radio::getPower() {}
void Radio::getVersion() {}

void Radio::sleep() {}

void Radio::reset() {}

void Radio::send(uint8_t data)
{
    pu_tx(data);
}

void Radio::send(uint8_t *data, int offset, int count)
{
    for (int i = 0; i < count; i++)
    {
        pu_tx(data[offset + i]);
    }
}

void Radio::request(uint8_t *buf, int offset, int count)
{
    for (int i = 0; i < count; i++)
    {
        buf[i] = pu_rx();
    }
}