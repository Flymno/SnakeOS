#ifndef PORTS_H_
#define PORTS_H_

/* ---------------- Includes ---------------- */
#include <stdint.h>

/* ---------------- Public API Prototypes ---------------- 
* uint8_t inb(uint16_t port)
*   -Reads a single byte from the specified I/O port
* void outb(uint16_t port, uint8_t val)
*   -Writes a single byte to the specified I/O port
*/
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);

#endif