#ifndef HAL_CONSOLE_H_
#define HAL_CONSOLE_H_

typedef struct 
{
    void (*init)(void);
    void (*put_char)(char c);
} HALConsole_t;

#endif