
#include "sunos.h"

#define SERIAL_COM1 0x3F8

#define SERIAL_DATA       (SERIAL_COM1 + 0)
#define SERIAL_INTERRUPT  (SERIAL_COM1 + 1)
#define SERIAL_FIFO       (SERIAL_COM1 + 2)
#define SERIAL_LINE       (SERIAL_COM1 + 3)
#define SERIAL_MODEM      (SERIAL_COM1 + 4)
#define SERIAL_STATUS     (SERIAL_COM1 + 5)

static int serial_initialized = 0;

void serial_init(void)
{
    /* Disable serial interrupts */
    outb(SERIAL_INTERRUPT, 0x00);

    /* Enable divisor latch */
    outb(SERIAL_LINE, 0x80);

    /* Set baud rate to 38400 */
    outb(SERIAL_DATA, 0x03);
    outb(SERIAL_INTERRUPT, 0x00);

    /* 8 data bits, no parity, one stop bit */
    outb(SERIAL_LINE, 0x03);

    /* Enable FIFO */
    outb(SERIAL_FIFO, 0xC7);

    /* Enable modem control */
    outb(SERIAL_MODEM, 0x0B);

    serial_initialized = 1;
}

static int serial_transmit_ready(void)
{
    return (inb(SERIAL_STATUS) & 0x20) != 0;
}

void serial_write_char(char character)
{
    if (!serial_initialized)
        return;

    while (!serial_transmit_ready())
    {
        __asm__ volatile("pause");
    }

    outb(SERIAL_DATA, (uint8_t)character);
}

void serial_write(const char *text)
{
    if (!text)
        return;

    while (*text)
    {
        if (*text == '\n')
            serial_write_char('\r');

        serial_write_char(*text++);
    }
}

void serial_write_hex(uint64_t value)
{
    static const char digits[] =
        "0123456789ABCDEF";

    serial_write("0x");

    for (int i = 60; i >= 0; i -= 4)
    {
        uint8_t digit = (value >> i) & 0xF;

        serial_write_char(digits[digit]);
    }
}
