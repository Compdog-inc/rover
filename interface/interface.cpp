#include <framework.h>
#include <ioutils.h>
#include <usart.h>
#include <serialdebug.h>
#include <clock.h>
#include <timer.h>
#include <radio.h>

DebugInterface debug;

Clock clock;
IOPort io = io_port_default;

#define LED_PIN 13

int main()
{
    io.reset();

    debug = DebugInterface("Interface", CURRENT_VERSION);
    debug.printHeader();

    clock.init();
    sei();

    Timer timer(&clock);
    Radio radio = Radio(&io);

    io.set_dir(LED_PIN, IODir::Out);

    debug.info_P(PSTR("Setting up radio\n"));
    radio.enterSetup();
    radio.enable();
    timer.spinWait(Time::fromSeconds(0.4f));

    radio.setChannel(42);
    radio.setBaud(9600L);
    radio.setMode(RadioMode::Normal);
    radio.setPower(RADIO_POWER_20);

    debug.info_P(PSTR("Radio setup done\n"));

    radio.exitSetup();
    timer.spinWait(Time::fromSeconds(0.8f));

    debug.info_P(PSTR("Radio ready\n"));

    radio.send(0xAB);
    radio.send(0xCD);
    radio.send(0xEF);
    radio.send(0x13);
}