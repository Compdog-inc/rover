#include <framework.h>
#include <drivetrain.h>
#include <ioutils.h>
#include <usart.h>

Drivetrain drivetrain;
IOPort io = io_port_default;

#define LED_PIN 13

int main()
{
    drivetrain.enable();
    USART::enable();
    USART::setBaudRate(115200);
    USART::redirectStdout();
    sei();

    printf("USART test!\n");

    io.reset();
    io.set_dir(LED_PIN, IODir::Out);

    while (1)
    {
        // Drivetrain test
        printf("requesting update\n");
        drivetrain.requestUpdate();
        float power = drivetrain.getLeftPower();
        printf("Left power: %f\n", power);
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