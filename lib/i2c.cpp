#include "framework.h"
#include "i2c.h"
#include <util/twi.h>
#include "serialdebug.h"
#include "staticlist.h"

static StaticList<TWIStatus> suppress_list(TWI_SUPPRESS_MAX_COUNT);

#define WAIT_FOR_TWINT()           \
    while (!(TWCR & (1 << TWINT))) \
        ;

#define CLEAR_TWINT() TWCR |= (1 << TWINT);

DebugInterface dbg("I2C", Version(256));

bool isSuppressed(TWIStatus status)
{
    return suppress_list.Find(status) != -1;
}

void TWI::enable()
{
    TWCR |= (1 << TWEN) | (0 << TWIE);
    TWBR = 12;
}

void TWI::disable()
{
    TWCR &= ~((1 << TWEN) | (0 << TWIE));
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

    uint8_t status = TW_STATUS;
    if (status != TW_START && status != TW_REP_START)
    {
        if (!isSuppressed((TWIStatus)status))
            dbg.error("sendStart status '%s'\n", TWI::nameOfStatus((TWIStatus)status));
        return false;
    }
    return true;
}

bool TWI::sendTo(uint8_t address)
{
    if (!sendStart())
        return false;
    // send SLA+W
    TWDR = (address << 1) + TW_WRITE;
    TWCR &= ~(1 << TWSTA);
    CLEAR_TWINT();
    WAIT_FOR_TWINT();

    // check status
    uint8_t status = TW_STATUS;
    if (status != TW_MT_SLA_ACK)
    {
        if (!isSuppressed((TWIStatus)status))
            dbg.error("sendTo status '%s'\n", nameOfStatus((TWIStatus)status));
        return false;
    }
    else
    {
        return true;
    }
}

bool TWI::requestFrom(uint8_t address)
{
    if (!sendStart())
        return false;
    // send SLA+R
    TWDR = (address << 1) + TW_READ;
    TWCR &= ~(1 << TWSTA);
    CLEAR_TWINT();
    WAIT_FOR_TWINT();

    // check status
    uint8_t status = TW_STATUS;
    if (status != TW_MR_SLA_ACK)
    {
        if (!isSuppressed((TWIStatus)status))
            dbg.error("requestFrom status '%s'\n", nameOfStatus((TWIStatus)status));
        return false;
    }
    else
    {
        // connect to I2C bus in read mode
        connect();
        return true;
    }
}

TWIStatus TWI::nextStatus()
{
    CLEAR_TWINT();
    WAIT_FOR_TWINT();
    return (TWIStatus)TW_STATUS;
}

void TWI::endTransfer()
{
    TWCR |= (1 << TWSTO);
    disconnect();
    CLEAR_TWINT();
}

void TWI::resetState()
{
    TWCR |= (1 << TWSTO);
    CLEAR_TWINT();
}

bool TWI::readAvailable()
{
    bool hasTwint = (TWCR & (1 << TWINT));
    uint8_t status = TW_STATUS;

    if (hasTwint && status == TW_SR_STOP)
        CLEAR_TWINT();
    return status == TW_SR_SLA_ACK;
}

bool TWI::isDataRequested()
{
    if (readAvailable())
        return false;

    if (TW_STATUS == TW_ST_SLA_ACK)
    {
        return true;
    }

    return false;
}

int TWI::readByte()
{
    CLEAR_TWINT();
    WAIT_FOR_TWINT();
    uint8_t status = TW_STATUS;
    uint8_t data = TWDR;

    switch (status)
    {
    case TW_MR_DATA_ACK:
    case TW_SR_DATA_ACK:
    case TW_SR_GCALL_DATA_ACK:
        return data;
    }

    if (!isSuppressed((TWIStatus)status))
        dbg.error("read status '%s'\n", nameOfStatus((TWIStatus)status));

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

bool TWI::write(uint8_t *data, int count, bool last)
{
    for (int i = 0; i < count; i++)
    {
        TWDR = data[i];
        if (last && i == count - 1)
        {
            disconnect();
        }
        CLEAR_TWINT();
        WAIT_FOR_TWINT();
        if (last && i == count - 1)
        {
            connect();
        }
        uint8_t status = TW_STATUS;
        if (status != TW_ST_SLA_ACK &&
            status != TW_ST_ARB_LOST_SLA_ACK &&
            status != TW_ST_DATA_ACK &&
            status != TW_MT_DATA_ACK &&
            status != TW_MT_SLA_ACK &&
            status != TW_MT_DATA_NACK)
        {
            if (!isSuppressed((TWIStatus)status))
                dbg.error("write status '%s'\n", nameOfStatus((TWIStatus)status));
            return false;
        }
    }
    return true;
}

bool TWI::write(uint8_t *data, int count)
{
    return write(data, count, false);
}

const char *TWI::nameOfStatus(TWIStatus status)
{
    switch (status)
    {
    case TWIStatus::Start:
        return "TW_START";
    case TWIStatus::RepStart:
        return "TW_REP_START";
    case TWIStatus::MtSlaAck:
        return "TW_MT_SLA_ACK";
    case TWIStatus::MtSlaNack:
        return "TW_MT_SLA_NACK";
    case TWIStatus::MtDataAck:
        return "TW_MT_DATA_ACK";
    case TWIStatus::MtDataNack:
        return "TW_MT_DATA_NACK";
    case TWIStatus::MtArbLost:
        return "TW_M*_ARB_LOST";
    case TWIStatus::MrSlaAck:
        return "TW_MR_SLA_ACK";
    case TWIStatus::MrSlaNack:
        return "TW_MR_SLA_NACK";
    case TWIStatus::MrDataAck:
        return "TW_MR_DATA_ACK";
    case TWIStatus::MrDataNack:
        return "TW_MR_DATA_NACK";
    case TWIStatus::StSlaAck:
        return "TW_ST_SLA_ACK";
    case TWIStatus::StArbLostSlaAck:
        return "TW_ST_ARB_LOST_SLA_ACK";
    case TWIStatus::StDataAck:
        return "TW_ST_DATA_ACK";
    case TWIStatus::StDataNack:
        return "TW_ST_DATA_NACK";
    case TWIStatus::StLastData:
        return "TW_ST_LAST_DATA";
    case TWIStatus::SrSlaAck:
        return "TW_SR_SLA_ACK";
    case TWIStatus::SrArbLostSlaAck:
        return "TW_SR_ARB_LOST_SLA_ACK";
    case TWIStatus::SrGcallAck:
        return "TW_SR_GCALL_ACK";
    case TWIStatus::SrArbLostGcallAck:
        return "TW_SR_ARB_LOST_GCALL_ACK";
    case TWIStatus::SrDataAck:
        return "TW_SR_DATA_ACK";
    case TWIStatus::SrDataNack:
        return "TW_SR_DATA_NACK";
    case TWIStatus::SrGcallDataAck:
        return "TW_SR_GCALL_DATA_ACK";
    case TWIStatus::SrGcallDataNack:
        return "TW_SR_GCALL_DATA_NACK";
    case TWIStatus::Stop:
        return "TW_SR_STOP";
    case TWIStatus::NoInfo:
        return "TW_NO_INFO";
    case TWIStatus::BusError:
        return "TW_BUS_ERROR";
    default:
        return "UNKNOWN";
    }
}

void TWI::suppress(TWIStatus status)
{
    suppress_list.Add(status);
}

void TWI::unsuppress(TWIStatus status)
{
    int ind = suppress_list.Find(status);
    if (ind != -1)
        suppress_list.Remove(ind);
}