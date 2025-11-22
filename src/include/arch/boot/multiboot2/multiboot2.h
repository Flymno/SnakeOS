#ifndef MULTIBOOT2_H_
#define MULTIBOOT2_H_

#include <stdint.h>

#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB 1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 2

int verify_multiboot(uint32_t multiboot2_magic);

void multiboot2_init(uint32_t multiboot2_info_addr);

#endif