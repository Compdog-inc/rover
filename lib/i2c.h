#ifndef TWI_H
#define TWI_H

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
    /// @brief Masks the address comparison result (mask 010 will match addresses 111,010,110,011)
    /// @param mask 7-bit mask
    void setAddressMask(uint8_t mask);

    /// @brief Clears START/STOP registers to prepare for SR/ST modes
    void setSlave();

    /// @brief Sends SLA+W request as master to the specified address
    void sendTo(uint8_t address);
    /// @brief Sends SLA+R request as master to the specified address
    void requestFrom(uint8_t address);
    /// @brief Ends current transfer of data in master mode (SLA+R/SLA+W)
    void endTransfer();

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
}

#endif