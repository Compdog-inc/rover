#include <framework.h>
#include <constants.h>
#include <ioutils.h>
#include <clock.h>
#include <i2c.h>
#include <serialdebug.h>
#include <timer.h>

DebugInterface debug;

IOPort io = io_port_default;

#define LED_PIN 13

Clock clock;

int main()
{
    debug = DebugInterface("Environment", CURRENT_VERSION);
    debug.printHeader();

    clock.init();
    TWI::enable();
    TWI::connect();
    TWI::disableGeneralCall();
    TWI::setAddress(ENVIRONMENT_I2C);
    TWI::setAddressMask(0x00);
    TWI::setSlave();

    io.reset();
    io.set_dir(LED_PIN, IODir::Out);

    Timer timer(&clock);

    while (1)
    {
        timer.spinWait(Time::fromSeconds(0.03f));
        io.put(LED_PIN, true);
        timer.spinWait(Time::fromSeconds(0.03f));
        io.put(LED_PIN, false);
    }
}