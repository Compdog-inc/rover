#include "framework.h"
#include "i2c.h"
#include <util/twi.h>
#include "internal/i2cmaster.h"
#include "internal/i2cslave.h"
#include "serialdebug.h"

static bool isTransfering = false;
static bool isSlave = false;
static volatile bool slaveRequested = false;
#define RECV_BUFFER_SIZE 16
static volatile uint8_t recv_buffer[RECV_BUFFER_SIZE];
static volatile uint8_t recv_buffer_put_pos = 0;
static volatile uint8_t recv_buffer_get_pos = 0;
static volatile uint8_t recv_buffer_len = 0;

void recv(uint8_t data)
{
    recv_buffer[recv_buffer_put_pos++] = data;
    recv_buffer_len++;
    if (recv_buffer_put_pos >= RECV_BUFFER_SIZE)
    {
        recv_buffer_put_pos = 0;
    }
}

void req()
{
    slaveRequested = true;
}

static int sput(uint8_t data, ByteStream *stream)
{
    if (isSlave)
    {
        I2C_transmitByte(data);
        return 1;
    }
    else
    {
        if (i2c_write(data) == 1)
        {
            return -1;
        }
        return 1;
    }
}

static int sget(ByteStream *stream, bool last)
{
    if (isSlave)
    {
        if (recv_buffer_len > 0)
        {
            uint8_t d = recv_buffer[recv_buffer_get_pos++];
            recv_buffer_len--;
            if (recv_buffer_get_pos >= RECV_BUFFER_SIZE)
            {
                recv_buffer_get_pos = 0;
            }
            return d;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if (last)
        {
            isTransfering = false;
        }
        return i2c_read(!last);
    }
}

static int slen(ByteStream *stream)
{
    if (isSlave)
    {
        return recv_buffer_len;
    }
    else
    {
        return -1;
    }
}

void TWI::enable()
{
    isSlave = false;
    i2c_init();
    sei();
}

void TWI::enable(uint8_t address)
{
    isSlave = true;
    I2C_setCallbacks(recv, req);
    I2C_init(address);
    sei();
}

void TWI::disable()
{
    if (isSlave)
    {
        isSlave = false;
        I2C_stop();
    }
    else
    {
        TWCR = 0;
        TWBR = 0;
    }
}

bool TWI::sendTo(uint8_t address)
{
    if (!isTransfering)
    {
        i2c_start_wait((address & 0xFE) + I2C_WRITE);
        isTransfering = true;
        return true;
    }
    else
    {
        return i2c_rep_start((address & 0xFE) + I2C_WRITE) == 0;
    }
}

bool TWI::requestFrom(uint8_t address)
{
    if (!isTransfering)
    {
        i2c_start_wait((address & 0xFE) + I2C_READ);
        isTransfering = true;
        return true;
    }
    else
    {
        return i2c_rep_start((address & 0xFE) + I2C_READ) == 0;
    }
}

TWIStatus TWI::getStatus()
{
    return (TWIStatus)TW_STATUS;
}

void TWI::endTransfer()
{
    i2c_stop();
    isTransfering = false;
}

ByteStream TWI::getStream()
{
    return ByteStream(sput, sget, slen);
}

bool TWI::isDataRequested()
{
    return slaveRequested;
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