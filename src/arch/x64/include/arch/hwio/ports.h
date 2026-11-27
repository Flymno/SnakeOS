#ifndef PORTS_H_
#define PORTS_H_

/* ---------------- Includes ---------------- */
#include <common/types.h>

/* ---------------- Public API Prototypes ---------------- 
* uint8_t inb(uint16_t port)
*   -Reads a single byte from the specified I/O port
* void outb(uint16_t port, uint8_t val)
*   -Writes a single byte to the specified I/O port
*/
u8 inb(u16 port);
void outb(u16 port, u8 val);

#endif