#include <common/types.h>
#include <hal/hal.h>
#include <lib/string.h>
#include <drivers/console.h>

void kprint_char(char c) {
    if (HAL->active_console && HAL->active_console->control) {
        HAL->active_console->control(CMD_PUT, (hal_arg_t)c, 0);
    }
}

void kprint_string(const char* s) {
    if (HAL->active_console && HAL->active_console->control) {
        HAL->active_console->control(CMD_WRITE, (hal_arg_t)s, 0);
    }
}
