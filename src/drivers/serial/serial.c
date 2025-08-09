#include <stddef.h>
#include <stdint.h>
#include "drivers/serial/serial.h"
#include "lib/string.h"
#include "arch/hwio/ports.h"

void init_serial() {
	outb(0x3F8 + 1, 0x00);    // Disable all interrupts
    outb(0x3F8 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(0x3F8 + 0, 0x03);    // Set divisor to 3 (low byte) 38400 baud
    outb(0x3F8 + 1, 0x00);    //                 (high byte)
    outb(0x3F8 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(0x3F8 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(0x3F8 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int is_transmit_empty() {
	return inb(0x3f8 + 5) & 0x20;
 }

void serial_putchar(char c) {
	while (is_transmit_empty() == 0);
	outb(0x3f8, c);
}

void serial_writestring(const char* data) {
	size_t size = strlen(data);
	for (size_t i = 0; i < size; i++)
		serial_putchar(data[i]);
}

void serial_newline() {
    serial_writestring("\n");
}

void serial_writehex(uint32_t val) {
    char buf[11] = "0x00000000";
    const char* hex_chars = "0123456789ABCDEF";
    for (int i = 0; i < 8; i++) {
        buf[9 - i] = hex_chars[val & 0xF];
        val >>= 4;
    }
    serial_writestring(buf);
}

void serial_writedec(uint32_t val) {
    char buf[12];
    for (int i = 0; i < 12; i++) {
        char digit;
        digit = val % 10;
        digit += '0';
        buf[12 - i] = digit;
        val /= 10;
    }
    serial_writestring(buf);
}