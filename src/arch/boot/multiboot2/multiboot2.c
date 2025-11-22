#include <stdint.h>
#include "arch/boot/multiboot2/multiboot2.h"
#include "drivers/serial/serial.h"
#include "arch/memory/memoryMap.h"
#include "arch/graphics/framebuffer.h"

#define MULTIBOOT_TAG_TYPE_END 0
#define MULTIBOOT_TAG_FRAMEBUFFER 8
#define MULTIBOOT_TAG_MMAP 6
#define MULTIBOOT2_MAGIC 0x36d76289

struct multiboot2_tag
{
	uint32_t type;
	uint32_t size;
};

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
  struct 
  {
	uint8_t framebuffer_red_field_position;
    uint8_t framebuffer_red_mask_size;
    uint8_t framebuffer_green_field_position;
    uint8_t framebuffer_green_mask_size;
    uint8_t framebuffer_blue_field_position;
    uint8_t framebuffer_blue_mask_size;
  };
  
};

struct multiboot2_tag_mmap
{
  uint32_t type;
  uint32_t size;
  uint32_t entry_size;
  uint32_t entry_version;
  memoryRegion entries[0];  
};

int verify_multiboot(uint32_t multiboot2_magic) {
	if (multiboot2_magic == MULTIBOOT2_MAGIC) { return 0;}
	return 1;
}

void multiboot2_mmap(struct multiboot2_tag *memory_map_tag) {
	memoryRegion *mmapEntry;
	for (mmapEntry = ((struct multiboot2_tag_mmap *) memory_map_tag)->entries; (uint8_t *) mmapEntry < (uint8_t * ) memory_map_tag + memory_map_tag->size; mmapEntry = (memoryRegion *)((uintptr_t) mmapEntry + ((struct multiboot2_tag_mmap *) memory_map_tag)->entry_size)) {
		add_mmap_entry(mmapEntry);
	}
}

void multiboot2_framebuffer(struct multiboot2_tag *tag) {
	struct multiboot2_tag_framebuffer* frame_buffer_tag = (struct multiboot2_tag_framebuffer*) tag;
	
	framebuffer.addr = frame_buffer_tag->framebuffer_addr;
	framebuffer.pitch = frame_buffer_tag->framebuffer_pitch;
	framebuffer.width = frame_buffer_tag->framebuffer_width;
	framebuffer.height = frame_buffer_tag->framebuffer_height;
	framebuffer.bpp = frame_buffer_tag->framebuffer_bpp;
	framebuffer.type = frame_buffer_tag->framebuffer_type;

	if (framebuffer.type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
		framebuffer.red_pos = frame_buffer_tag->framebuffer_red_field_position;
		framebuffer.green_pos = frame_buffer_tag->framebuffer_green_field_position;
		framebuffer.blue_pos = frame_buffer_tag->framebuffer_blue_field_position;
	} else {
		framebuffer.red_pos = -1;
		framebuffer.green_pos = -1;
		framebuffer.blue_pos = -1;
	}
}

void multiboot2_init(const uint32_t multiboot2_info_addr) {
	struct multiboot2_tag *tag;
	
	for (tag = (struct multiboot2_tag *) (multiboot2_info_addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot2_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) {
		switch (tag->type)
		{
		case MULTIBOOT_TAG_FRAMEBUFFER:
			multiboot2_framebuffer(tag);
			break;
		case MULTIBOOT_TAG_MMAP:
			multiboot2_mmap(tag);
			break;
		default:
			break;
		}
	};
	
}