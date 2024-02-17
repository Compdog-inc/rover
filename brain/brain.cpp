#include <framework.h>
#include <drivetrain.h>
#include <ioutils.h>
#include <usart.h>
#include <serialdebug.h>

DebugInterface debug;

Drivetrain drivetrain;
IOPort io = io_port_default;

#define LED_PIN 13

int main()
{
    debug = DebugInterface("Brain", CURRENT_VERSION);
    debug.printHeader();

    drivetrain.enable();
    sei();

    io.reset();
    io.set_dir(LED_PIN, IODir::Out);

    drivetrain.drive(Direction::Forward);
    return;

    while (1)
    {
        // Drivetrain test
        debug.info("requesting update\r\n");
        drivetrain.requestUpdate();
        float power = drivetrain.getLeftPower();
        debug.info("Left power: %f\r\n", power);
        drivetrain.drive(Direction::Forward);
        drivetrain.requestUpdate();
        float powerD = drivetrain.getLeftPower();
        if (power != powerD)
        {
            while (1)
            {
                io.put(LED_PIN, true);
                _delay_ms(50);
                io.put(LED_PIN, false);
                _delay_ms(50);
            }
        }
        else
        {
            io.put(LED_PIN, true);
        }
        break;
    }
}