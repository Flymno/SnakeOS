#ifndef SERIAL_H_
#define SERIAL_H_

#include <hal/hal_console.h>

/* ---------------- Public API Implementation ---------------- 
* HALConsole_t* x86_serial_get_console()
*   -Populates the HAL interface with the x86 serial methods
*/
HALConsole_t* x86_serial_get_console();

#endif