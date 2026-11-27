/* ---------------- Includes ---------------- */
#include <common/types.h>
#include <arch/hwio/ports.h>

/* ---------------- Internal Constants ---------------- */

/* ---------------- Public API Implementation ---------------- 
* uint8_t inb(uint16_t port)
*   -Reads a single byte from the specified I/O port
* void outb(uint16_t port, uint8_t val)
*   -Writes a single byte to the specified I/O port
*/
u8 inb(u16 port){
	u8 ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
};

void outb(u16 port, u8 val){
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) :"memory");
};
