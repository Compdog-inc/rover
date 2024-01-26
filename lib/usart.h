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

    void redirectStdout();
}

#endif