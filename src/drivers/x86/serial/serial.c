/* ---------------- Includes ---------------- */
#include <stddef.h>
#include <stdint.h>
#include "drivers/x86/serial/serial.h"
#include "lib/string.h"
#include "arch/x86/hwio/ports.h"



/* ---------------- Internal Helper Prototypes ---------------- */
static uint8_t serial_can_write(void);
static void serial_putchar(char c);

/* ---------------- Internal Helper Implementation ---------------- 
* static uint8_t serial_can_write(void)
*   -Returns nonzero if a new byte can be written to the serial port
* static void serial_putchar(char c)
*   -Writes a byte to the serial port
*/
static uint8_t serial_can_write(void) {
	return inb(0x3f8 + 5) & 0x20;
 }

static void serial_putchar(char c) {
	while (serial_can_write() == 0);
	outb(0x3f8, c);
}

/* ---------------- Public API Implementation ---------------- 
* void serial_init()
*   -Initialises the serial port hardware (COM1)
* void serial_writestring(const char* data)
*   -Writes a null-terminated string to the serial port
* void serial_newline(void)
*   -Writes a newline ("\n") to the serial port
* void serial_writehex(uint32_t val)
*   -Writes a 32-bit value as hexadecimal (0xXXXXXXXX)
* void serial_writedec(uint32_t val)
*   -Writes a 32-bit value as decimal
*/
void serial_init() {
	outb(0x3F8 + 1, 0x00);    // Disable all interrupts
    outb(0x3F8 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(0x3F8 + 0, 0x03);    // Set divisor to 3 (low byte) 38400 baud
    outb(0x3F8 + 1, 0x00);    //                 (high byte)
    outb(0x3F8 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(0x3F8 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(0x3F8 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void serial_writestring(const char* data) {
	size_t size = strlen(data);
	for (size_t i = 0; i < size; i++)
		serial_putchar(data[i]);
}

void serial_newline(void) {
    serial_writestring("\n");
}

void serial_writehex(uint32_t val) {
    char buf[11] = "0x00000000";
    const char* hexChars = "0123456789ABCDEF";
    for (int i = 0; i < 8; i++) {
        buf[9 - i] = hexChars[val & 0xF];
        val >>= 4;
    }
    serial_writestring(buf);
}

void serial_writedec(uint32_t val) {
    char buf[12];
    int pos = 0;

    if (val == 0) {
        buf[pos++] = '0';
    } else {
        char tmp[10];
        int tmpPos = 0;
        while (val > 0) {
            tmp[tmpPos++] = '0' + (val % 10);
            val /= 10;
        }
        for (int i = tmpPos - 1; i >= 0; i--) {
            buf[pos++] = tmp[i];
        }
    }
    buf[pos] = '\0';
    serial_writestring(buf);
}
