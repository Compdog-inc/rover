#include <framework.h>
#include <constants.h>
#include <ioutils.h>
#include <clock.h>
#include <i2c.h>
#include <serialdebug.h>

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
    TWI::setAddressMask(0xFF);
    TWI::setSlave();

    io.reset();
    io.set_dir(LED_PIN, IODir::Out);

    while (1)
    {
        _delay_ms(30);
        io.put(LED_PIN, true);
        _delay_ms(30);
        io.put(LED_PIN, false);
    }
}