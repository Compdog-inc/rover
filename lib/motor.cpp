#include "framework.h"
#include "ioutils.h"
#include "motor.h"

IOPort io = io_port_default;

Motor::Motor(uint8_t pin1, uint8_t pin2)
{
    this->pin1 = pin1;
    this->pin2 = pin2;

    io.set_dir(pin1, IODir::Out);
    io.set_dir(pin2, IODir::Out);
    stop();
}

void Motor::clockwise()
{
    io.put(pin1, true);
    io.put(pin2, false);
}

void Motor::counterclockwise()
{
    io.put(pin1, false);
    io.put(pin2, true);
}

void Motor::stop()
{
    io.put(pin1, BRAKE_MODE);
    io.put(pin2, BRAKE_MODE);
}