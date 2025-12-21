/* ---------------- Includes ---------------- */
#include <stdint.h>
#include "arch/x86/hwio/ports.h"

/* ---------------- Internal Constants ---------------- */
#define PIC1 0x20
#define PIC2 0xA0
#define ICW1 0x11
#define ICW4 0x01

/* ---------------- Public API Implementation ---------------- 
* uint8_t inb(uint16_t port)
*   -Reads a single byte from the specified I/O port
* void outb(uint16_t port, uint8_t val)
*   -Writes a single byte to the specified I/O port
*/
uint8_t inb(uint16_t port){
	uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
};

void outb(uint16_t port, uint8_t val){
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) :"memory");
};
