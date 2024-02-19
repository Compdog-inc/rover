#include <framework.h>
#include <drivetrain.h>
#include <ioutils.h>
#include <usart.h>
#include <serialdebug.h>
#include <clock.h>
#include <timer.h>

DebugInterface debug;

Clock clock;
Drivetrain drivetrain;
IOPort io = io_port_default;

#define LED_PIN 13

int main()
{
    debug = DebugInterface("Brain", CURRENT_VERSION);
    debug.printHeader();

    clock.init();
    drivetrain.enable();
    sei();

    io.reset();
    io.set_dir(LED_PIN, IODir::Out);

    Timer timer(&clock);

    while (1)
    {
        drivetrain.drive(Direction::Forward);
        for (int i = 0; i < 30; i++)
        {
            drivetrain.requestUpdate();
            drivetrain.logTelemetry();
            timer.spinWait(Time::fromSeconds(0.03f));
        }
        drivetrain.drive(Direction::Backward);
        for (int i = 0; i < 30; i++)
        {
            drivetrain.requestUpdate();
            drivetrain.logTelemetry();
            timer.spinWait(Time::fromSeconds(0.03f));
        }
        drivetrain.stop();
        for (int i = 0; i < 30; i++)
        {
            drivetrain.requestUpdate();
            drivetrain.logTelemetry();
            timer.spinWait(Time::fromSeconds(0.03f));
        }
    }
}