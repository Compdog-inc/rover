#include "framework.h"
#include "usart.h"

#ifndef BAUD_TOL
#define BAUD_TOL 2
#endif

void USART::enable()
{
    UCSR0A = 0;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // enable TX/RX
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); // 8-bit data
}

void USART::disable()
{
    UCSR0A = 0;
    UCSR0B = 0;
    UCSR0C = 0;
}

void USART::setBaudRate(unsigned long baud)
{
    unsigned long ubbr = (((F_CPU) + 8UL * (baud)) / (16UL * (baud)) - 1UL);
    if (
        (100 * (F_CPU) > (16 * ((ubbr) + 1)) * (100 * (baud) + (baud) * (BAUD_TOL))) ||
        (100 * (F_CPU) <
         (16 * ((ubbr) + 1)) * (100 * (baud) - (baud) * (BAUD_TOL))))
    {
        ubbr = (((F_CPU) + 4UL * (baud)) / (8UL * (baud)) - 1UL);
        UCSR0A |= (1 << U2X0);
    }
    else
    {
        UCSR0A &= ~(1 << U2X0);
    }

    UBRR0H = ubbr >> 8;
    UBRR0L = ubbr & 0xff;
}

void USART::write(uint8_t *data, int count)
{
    for (int i = 0; i < count; i++)
    {
        while (!(UCSR0A & (1 << UDRE0)))
            ;

        UDR0 = data[i];
    }
}

int writeChar(char ch, __file *file __attribute__((unused)))
{
    uint8_t bt = (uint8_t)ch;
    USART::write(&bt, 1);
    return 1;
}

static FILE usart_stdout = {0, 0, _FDEV_SETUP_WRITE, 0, 0, writeChar, 0, 0};

void USART::redirectStdout()
{
    stdout = &usart_stdout;
}