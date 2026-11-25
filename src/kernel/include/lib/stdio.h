#ifndef STDIO_H_
#define STDIO_H_

#include <common/types.h>
#include <stdarg.h>

size_t vprintf(const char* fmt, va_list args);
size_t printf(const char* fmt, ...);

#endif