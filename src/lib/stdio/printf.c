#include <stdarg.h>
#include <common/types.h>
#include <drivers/console.h>
#include <lib/stdio.h>
#include <lib/conv.h>
#include <lib/string.h>

#define PRINTF_FLAG_ALTERNATE 1
#define PRINTF_FLAG_ZERO (1 << 1)
#define PRINTF_FLAG_LEFT (1 << 2)
#define PRINTF_FLAG_BLANK (1 << 3)
#define PRINTF_FLAG_SIGN (1 << 4)

size_t vprintf(const char* fmt, va_list args) {
    size_t written = 0;

    while (*fmt != '\0')
    {
        if(*fmt == '%') {
            fmt++;

            u8 flagSearching = 1;
            u8 flags = 0;

            while (flagSearching) {
                switch (*fmt)
                {
                case '#':
                    flags |= PRINTF_FLAG_ALTERNATE;
                    fmt++;
                    break;
                case '0':
                    flags |= PRINTF_FLAG_ZERO;
                    fmt++;
                    break;
                case '-':
                    flags |= PRINTF_FLAG_LEFT;
                    fmt++;
                    break;
                case ' ':
                    flags |= PRINTF_FLAG_BLANK;
                    fmt++;
                    break;
                case '+':
                    flags |= PRINTF_FLAG_SIGN;
                    fmt++;
                    break;
                default:
                    flagSearching = 0;
                    break;
                }
            }

            size_t width = 0;
            char paddChar = (flags & PRINTF_FLAG_ZERO) ? '0' : ' ';

            while (*fmt >= '0' && *fmt <= '9') {
                width = (width * 10) + (*fmt - '0');
                fmt++;
            }

            if (*fmt == '*') {
                width = va_arg(args, size_t);
                fmt++;
            }

            switch (*fmt) {
            case 's': {
                char* s = va_arg(args, char*);

                kprint_string(s);
                written += strlen(s);
                break;
            }

            case 'x': {
                char buf[32];
                u32 val = va_arg(args, u32);

                itoa(val, buf, 16);

                if (flags & PRINTF_FLAG_ALTERNATE) {
                    kprint_string("0x");
                    written += 2;
                }

                size_t len = strlen(buf);

                if (len < width) {
                    for (size_t i = 0; i < (width - len); i++) {
                        kprint_char(paddChar);
                        written++;
                    }
                }

                kprint_string(buf);
                written += len;
                break;
            }

            case 'X': {
                char buf[32];
                u32 val = va_arg(args, u32);

                itoa(val, buf, 16);

                if (flags & PRINTF_FLAG_ALTERNATE) {
                    kprint_string("0X");
                    written += 2;
                }

                size_t len = strlen(buf);

                if (len < width) {
                    for (size_t i = 0; i < (width - len); i++) {
                        kprint_char(paddChar);
                        written++;
                    }
                }

                kprint_string(buf);
                written += len;
                break;
            }

            case 'd': {
                char buf[32];
                s32 val = va_arg(args, s32);

                if (val < 0) {
                    kprint_char('-');
                    written++;
                    itoa((u32)-val, buf, 10);
                } else if ((flags & PRINTF_FLAG_SIGN)) {
                    kprint_char('+');
                    written++;
                    itoa((u32)val, buf, 10);
                } else {
                    itoa((u32)val, buf, 10);
                }

                size_t len = strlen(buf);

                if (len < width) {
                    for (size_t i = 0; i < (width - len); i++) {
                        kprint_char(paddChar);
                        written++;
                    }
                }

                kprint_string(buf);
                written += len;
                break;
            }

            case 'i': {
                char buf[32];
                s32 val = va_arg(args, s32);

                if (val < 0) {
                    kprint_char('-');
                    written++;
                    itoa((u32)-val, buf, 10);
                } else if ((flags & PRINTF_FLAG_SIGN)) {
                    kprint_char('+');
                    written++;
                    itoa((u32)val, buf, 10);
                } else {
                    itoa((u32)val, buf, 10);
                }

                size_t len = strlen(buf);

                if (len < width) {
                    for (size_t i = 0; i < (width - len); i++) {
                        kprint_char(paddChar);
                        written++;
                    }
                }

                kprint_string(buf);
                written += len;
                break;
            }
            
            case 'u': {
                char buf[32];
                u32 val = va_arg(args, u32);

                itoa(val, buf, 10);

                size_t len = strlen(buf);

                if (flags & PRINTF_FLAG_SIGN) {
                    kprint_char('+');
                    written++;
                }

                if (len < width) {
                    for (size_t i = 0; i < (width - len); i++) {
                        kprint_char(paddChar);
                        written++;
                    }
                }

                kprint_string(buf);
                written += len;
                break;
            }

            case '%': {
                kprint_char('%');
                written++;
            }

            default:
                break;
            }

        } else {
            kprint_char(*fmt);
            written++;
        }
        fmt++;
    }
    
    return written;
}

size_t printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    size_t written = vprintf(fmt, args);

    va_end(args);
    return written;
}