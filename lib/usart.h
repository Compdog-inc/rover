#ifndef USART_H
#define USART_H

#include <stdint.h>

namespace USART
{
    /// @brief Enable USART interface
    void enable();
    /// @brief Disable USART interface
    void disable();

    /// @brief Sets the baud rate
    void setBaudRate(unsigned long baud);

    /// @brief Wrties bytes from a buffer
    /// @param data Input buffer
    /// @param count Number of bytes to write
    void write(uint8_t *data, int count);

    /// @brief Creates a file stream and redirects stdout to the USART port (allows for printf)
    void redirectStdout();
}

#endif