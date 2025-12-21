#ifndef HAL_H_
#define HAL_H_

#include <hal/hal_console.h>

typedef struct
{
    HALConsole_t* console;
} HAL_t ;

extern HAL_t* HAL;

#endif