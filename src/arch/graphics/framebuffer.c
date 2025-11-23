#include <stdint.h>
#include <stddef.h>
#include "arch/graphics/framebuffer.h"
#include "arch/boot/multiboot2/multiboot2_consts.h"

framebuffer_t framebuffer = {0};

struct multiboot2_tag_framebuffer
{
  uint32_t type;
  uint32_t size;

  uint64_t framebuffer_addr;
  uint32_t framebuffer_pitch;
  uint32_t framebuffer_width;
  uint32_t framebuffer_height;
  uint8_t framebuffer_bpp;
  uint8_t framebuffer_type;
  uint16_t reserved;

  struct {
	uint8_t framebuffer_red_field_position;
    uint8_t framebuffer_red_mask_size;
    uint8_t framebuffer_green_field_position;
    uint8_t framebuffer_green_mask_size;
    uint8_t framebuffer_blue_field_position;
    uint8_t framebuffer_blue_mask_size;
  };
};

void framebuffer_multiboot2_init(const void* tag) {
	const struct multiboot2_tag_framebuffer* framebuffer_tag = tag;

	if (framebuffer_tag->type != TAG_FRAMEBUFFER) {
		return;
	}

	framebuffer.addr = framebuffer_tag->framebuffer_addr;
	framebuffer.pitch = framebuffer_tag->framebuffer_pitch;
	framebuffer.width = framebuffer_tag-> framebuffer_width;
	framebuffer.height = framebuffer_tag->framebuffer_height;
	framebuffer.bpp = framebuffer_tag->framebuffer_bpp;
	framebuffer.type = framebuffer_tag->framebuffer_type;

	if (framebuffer.type == FRAMEBUFFER_RGB) {
		framebuffer.red_pos = framebuffer_tag->framebuffer_red_field_position;
		framebuffer.green_pos = framebuffer_tag->framebuffer_green_field_position;
		framebuffer.blue_pos = framebuffer_tag->framebuffer_blue_field_position;
	} else {
		framebuffer.red_pos = -1;
		framebuffer.green_pos = -1;
		framebuffer.blue_pos = -1;
	}
}