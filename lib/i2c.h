#ifndef TWI_H
#define TWI_H

#ifndef TWI_SUPPRESS_MAX_COUNT
#define TWI_SUPPRESS_MAX_COUNT 16
#endif

enum class TWIStatus
{
    /// @brief Start condition transmitted
    Start = 0x08,
    /// @brief Repeated start condition transmitted
    RepStart = 0x10,
    /// @brief SLA+W transmitted, ACK received
    MtSlaAck = 0x18,
    /// @brief SLA+W transmitted, NACK received
    MtSlaNack = 0x20,
    /// @brief Data transmitted, ACK received
    MtDataAck = 0x28,
    /// @brief Data transmitted, NACK received
    MtDataNack = 0x30,
    /// @brief Arbitration lost in SLA+W or data
    MtArbLost = 0x38,
    /// @brief Arbitration lost in SLA+R or NACK
    MrArbLost = 0x38,
    /// @brief SLA+R transmitted, ACK received
    MrSlaAck = 0x40,
    /// @brief SLA+R transmitted, NACK received
    MrSlaNack = 0x48,
    /// @brief Data received, ACK returned
    MrDataAck = 0x50,
    /// @brief Data received, NACK returned
    MrDataNack = 0x58,
    /// @brief SLA+R received, ACK returned
    StSlaAck = 0xA8,
    /// @brief Arbitration lost in SLA+RW, SLA+R received, ACK returned
    StArbLostSlaAck = 0xB0,
    /// @brief Data transmitted, ACK received
    StDataAck = 0xB8,
    /// @brief Data transmitted, NACK received
    StDataNack = 0xC0,
    /// @brief Last data byte transmitted, ACK received
    StLastData = 0xC8,
    /// @brief SLA+W received, ACK returned
    SrSlaAck = 0x60,
    /// @brief Arbitration lost in SLA+RW, SLA+W received, ACK returned
    SrArbLostSlaAck = 0x68,
    /// @brief General call received, ACK returned
    SrGcallAck = 0x70,
    /// @brief Arbitration lost in SLA+RW, general call received, ACK returned
    SrArbLostGcallAck = 0x78,
    /// @brief Data received, ACK returned
    SrDataAck = 0x80,
    /// @brief Data received, NACK returned
    SrDataNack = 0x88,
    /// @brief General call data received, ACK returned
    SrGcallDataAck = 0x90,
    /// @brief General call data received, NACK returned
    SrGcallDataNack = 0x98,
    /// @brief Stop or repeated start condition received while selected
    Stop = 0xA0,
    /// @brief No state information available
    NoInfo = 0xF8,
    /// @brief Illegal start or stop condition
    BusError = 0x00
};

namespace TWI
{
    /// @brief Enable TWI interface and interrupts
    void enable();
    /// @brief Disable TWI interface and interrupts
    void disable();

    /// @brief Enable responding to address (virtually connecting the device)
    void connect();
    /// @brief Disable responding to address (virtually disconnecting the device)
    void disconnect();

    /// @brief Sets the TWI device address
    /// @param address 7-bit value
    void setAddress(uint8_t address);
    /// @brief Enable recognition of the general call address (0x00)
    void enableGeneralCall();
    /// @brief Disable recognition of the general call address (0x00)
    void disableGeneralCall();
    /// @brief Masks the address comparison result where 1 always matches (mask 101 will match addresses 111,010,110,011)
    /// @param mask 7-bit mask
    void setAddressMask(uint8_t mask);

    /// @brief Clears START/STOP registers to prepare for SR/ST modes
    void setSlave();

    /// @brief Sends SLA+W request as master to the specified address
    bool sendTo(uint8_t address);
    /// @brief Sends SLA+R request as master to the specified address
    bool requestFrom(uint8_t address);
    /// @brief Ends current transfer of data in master mode (SLA+R/SLA+W)
    void endTransfer();

    /// @brief Waits for next status update
    /// @return The new status
    TWIStatus nextStatus();

    /// @brief Resets slave state to unaddressed
    void resetState();

    /// @brief Read a single byte as a slave
    /// @return The received byte or -1 if an error occurred
    int readByte();
    /// @brief Reads bytes into a buffer as a slave
    /// @param output Output buffer
    /// @param count Number of bytes to read
    /// @return Number of bytes read until reached count or error
    int read(uint8_t *output, int count);
    /// @brief Returns true if TWINT is set
    bool readAvailable();
    /// @brief Returns true if current status is own SLA+R. Note: does not clear TWINT if returned true
    bool isDataRequested();

    /// @brief Wrties bytes from a buffer as a slave
    /// @param data Input buffer
    /// @param count Number of bytes to write
    /// @return True if successfull
    bool write(uint8_t *data, int count);

    /// @brief Wrties bytes from a buffer as a slave
    /// @param data Input buffer
    /// @param count Number of bytes to write
    /// @param last Should transmit LAST_DATA status on last byte
    /// @return True if successfull
    bool write(uint8_t *data, int count, bool last);

    const char *nameOfStatus(TWIStatus status);

    void suppress(TWIStatus status);
    void unsuppress(TWIStatus status);
}

#endif