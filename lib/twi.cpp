#include "framework.h"
#include "twi.h"

void TWI::enable()
{
    TWCR |= (1 << TWEN) | (1 << TWIE);
}

void TWI::disable()
{
    TWCR &= ~((1 << TWEN) | (1 << TWIE));
}