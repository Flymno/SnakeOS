#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <stdint.h>

typedef struct framebuffer {
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t  bpp;
    uint8_t  type;

    uint8_t red_pos;
    uint8_t green_pos;
    uint8_t blue_pos;

} framebuffer_t;

extern framebuffer_t framebuffer;

void framebuffer_multiboot2_init(const void* tag);

#endif