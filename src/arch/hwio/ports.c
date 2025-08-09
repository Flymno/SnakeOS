#include <stdint.h>
#include "arch/hwio/ports.h"

#define PIC1 0x20
#define PIC2 0xA0
#define ICW1 0x11
#define ICW4 0x01

uint8_t inb(uint16_t port){
	uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
};

void outb(uint16_t port, uint8_t val){
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) :"memory");
};
