#ifndef MULTIBOOT2_H_
#define MULTIBOOT2_H_

#include <stdint.h>

#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB 1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 2

typedef struct
{
	uintptr_t framebuffer_addr;
	uint32_t framebuffer_pitch;
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;
	uint8_t framebuffer_bpp;
	uint8_t framebuffer_type;
	uint8_t framebuffer_red_field_position;
	uint8_t framebuffer_green_field_position;
	uint8_t framebuffer_blue_field_position;

} multiboot2Info;

extern multiboot2Info multibootInfo;

int verify_multiboot(uint32_t multiboot2_magic);

void multiboot2_init(uint32_t multiboot2_info_addr);

#endif