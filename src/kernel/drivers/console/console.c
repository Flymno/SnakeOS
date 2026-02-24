#include <common/types.h>
#include <hal/hal.h>
#include <lib/string.h>
#include <drivers/console.h>

void kprint_char(char c) {
    if (HAL->console && HAL->console->put_char) {
        HAL->console->put_char(c);
    }
}

void kprint_string(const char* s) {
    for (size_t i = 0; s[i] != '\0'; i++) {
        kprint_char(s[i]);
    }
}
