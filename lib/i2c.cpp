#include "framework.h"
#include "i2c.h"
#include <util/twi.h>

#define WAIT_FOR_TWINT()           \
    while (!(TWCR & (1 << TWINT))) \
        ;

#define CLEAR_TWINT() TWCR |= (1 << TWINT);

void TWI::enable()
{
    TWCR |= (1 << TWEN) | (1 << TWIE);
    TWBR = 12;
}

void TWI::disable()
{
    TWCR &= ~((1 << TWEN) | (1 << TWIE));
    TWBR = 0;
}

void TWI::setAddress(uint8_t address)
{
    TWAR &= (1 << TWGCE);
    TWAR |= address << 1;
}

void TWI::enableGeneralCall()
{
    TWAR |= (1 << TWGCE);
}

void TWI::disableGeneralCall()
{
    TWAR &= ~(1 << TWGCE);
}

void TWI::setAddressMask(uint8_t mask)
{
    TWAMR = mask << 1;
}

void TWI::connect()
{
    TWCR |= (1 << TWEA);
}

void TWI::disconnect()
{
    TWCR &= ~(1 << TWEA);
}

void TWI::setSlave()
{
    TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
}

bool sendStart()
{
    TWCR |= (1 << TWSTA);
    CLEAR_TWINT();
    WAIT_FOR_TWINT();
    if (TW_STATUS != TW_START && TW_STATUS != TW_REP_START)
    {
        CLEAR_TWINT();
        return false;
    }
    return true;
}

void TWI::sendTo(uint8_t address)
{
    if (!sendStart())
        return;
    // send SLA+W
    TWDR = address + TW_WRITE;
    CLEAR_TWINT();
    WAIT_FOR_TWINT();

    // check status
    if (TW_STATUS != TW_MT_SLA_ACK)
        return;
}

void TWI::requestFrom(uint8_t address)
{
    if (!sendStart())
        return;
    // send SLA+R
    TWDR = address + TW_READ;
    CLEAR_TWINT();
    WAIT_FOR_TWINT();

    // check status
    if (TW_STATUS != TW_MR_SLA_ACK)
        return;
}

void TWI::endTransfer()
{
    TWCR |= (1 << TWSTO);
}

void TWI::resetState()
{
    TWCR |= (1 << TWSTO);
}

bool TWI::readAvailable()
{
    return (TWCR & (1 << TWINT));
}

bool TWI::isDataRequested()
{
    if (!readAvailable())
        return false;

    if (TW_STATUS != TW_ST_SLA_ACK)
    {
        CLEAR_TWINT();
        return false;
    }

    return true;
}

int TWI::readByte()
{
    WAIT_FOR_TWINT();
    uint8_t status = TW_STATUS;
    uint8_t data = TWDR;
    CLEAR_TWINT();
    switch (status)
    {
    case TW_MR_DATA_ACK:
    case TW_SR_DATA_ACK:
    case TW_SR_GCALL_DATA_ACK:
        return data;
    }

    return -1;
}

int TWI::read(uint8_t *output, int count)
{
    int i = 0;
    do
    {
        int tmp = readByte();
        if (tmp != -1)
        {
            output[i++] = (uint8_t)tmp;
        }
        else
        {
            break;
        }
    } while (i < count);
    return i;
}

bool TWI::write(uint8_t *data, int count)
{
    for (int i = 0; i < count; i++)
    {
        WAIT_FOR_TWINT();
        uint8_t status = TW_STATUS;
        if (status == TW_ST_SLA_ACK ||
            status == TW_ST_ARB_LOST_SLA_ACK ||
            status == TW_ST_DATA_ACK ||
            status == TW_MT_DATA_ACK ||
            status == TW_MT_SLA_ACK)
        {
            TWDR = data[i];
            CLEAR_TWINT();
        }
        else
        {
            CLEAR_TWINT();
            return false;
        }
    }

    return true;
}

ISR(TWI_vect)
{
    uint8_t status = TW_STATUS;
    printf("int: 0x%x\n", status);
    switch (status)
    {
    case TW_ST_LAST_DATA:
    {
        // clear interrupt
        TWCR |= (1 << TWINT);
        break;
    }
    }
}