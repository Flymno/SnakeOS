#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdint.h>

/* ---------------- Public API Prototypes ---------------- 
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
void serial_init(void);
void serial_writestring(const char* data);
void serial_newline(void);
void serial_writehex(uint32_t val);
void serial_writedec(uint32_t val);

#endif