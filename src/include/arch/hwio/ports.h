#ifndef PORTS_H_
#define PORTS_H_

#include <stdint.h>

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);

#endif