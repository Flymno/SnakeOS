/* ---------------- Includes ---------------- */
#include <stddef.h>
#include <stdint.h>
#include <arch/hwio/ports.h>
#include <hal/hal_console.h>

/* ---------------- Internal Helper Prototypes ---------------- */
static void x86_serial_init(void);
static void x86_serial_putchar(char c);
static size_t serial_can_write(void);

/* ---------------- Internal Helper Implementation ---------------- 
* static void serial_init()
*   -Initialises the serial port hardware (COM1)
* static void serial_putchar(char c)
*   -Writes a byte to the serial port
* static uint8_t serial_can_write(void)
*   -Returns nonzero if a new byte can be written to the serial port
*/
static void x86_serial_init(void) {
	outb(0x3F8 + 1, 0x00);    // Disable all interrupts
    outb(0x3F8 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(0x3F8 + 0, 0x03);    // Set divisor to 3 (low byte) 38400 baud
    outb(0x3F8 + 1, 0x00);    //                 (high byte)
    outb(0x3F8 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(0x3F8 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(0x3F8 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

static void x86_serial_putchar(char c) {
	while (serial_can_write() == 0);
	outb(0x3f8, c);
}

static size_t serial_can_write(void) {
	return inb(0x3f8 + 5) & 0x20;
 }

/* ---------------- HAL Interface Instance ---------------- */
static HALConsole_t x86SerialConsoleInstance = {
    .init = x86_serial_init,
    .put_char = x86_serial_putchar
};

/* ---------------- Public API Implementation ---------------- 
* HALConsole_t* x86_serial_get_console()
*   -Populates the HAL interface with the x86 serial methods
*/
HALConsole_t* x86_serial_get_console() {
    return &x86SerialConsoleInstance;
}