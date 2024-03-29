#include <framework.h>
#include <drivetrain.h>
#include <ioutils.h>
#include <usart.h>
#include <serialdebug.h>
#include <clock.h>
#include <timer.h>

DebugInterface debug;

Clock clock;
Drivetrain drivetrain(&clock);
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

    debug.info("Requesting update\n");
    bool ok = drivetrain.requestUpdate();
    debug.info("Result: %s\n", ok ? "OK" : "ERROR");
    _delay_ms(1000);
    // drivetrain.logTelemetry();

    return;

    while (1)
    {
        drivetrain.waitUntilAvailable();
        drivetrain.drive(Direction::Forward);
        drivetrain.waitUntilAvailable();
        drivetrain.setVelocity(-1.0f, 1.0f);
        drivetrain.waitUntilAvailable();
        timer.spinWait(Time::fromSeconds(1.0f));
        drivetrain.drive(Direction::Backward);
        drivetrain.waitUntilAvailable();
        drivetrain.setVelocity(1.0f, -1.0f);
        drivetrain.waitUntilAvailable();
        timer.spinWait(Time::fromSeconds(1.0f));
        drivetrain.stop();
        drivetrain.waitUntilAvailable();
        timer.spinWait(Time::fromSeconds(1.0f));
    }
}