#include <stdint.h>
#include "arch/boot/multiboot2/multiboot2.h"
#include "drivers/serial/serial.h"
#include "arch/memory/memoryMap.h"

#define MULTIBOOT_TAG_TYPE_END 0
#define MULTIBOOT_TAG_FRAMEBUFFER 8
#define MULTIBOOT_TAG_MMAP 6
#define MULTIBOOT2_MAGIC 0x36d76289

multiboot2Info multibootInfo = {0};

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
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB     1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT     2
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

void multiboot2_framebuffer(struct multiboot2_tag *frame_buffer_tag) {
	multibootInfo.framebuffer_addr = (uintptr_t) ((struct multiboot2_tag_framebuffer *) frame_buffer_tag)->framebuffer_addr;
	multibootInfo.framebuffer_pitch = ((struct multiboot2_tag_framebuffer *) frame_buffer_tag)->framebuffer_pitch;
	multibootInfo.framebuffer_width = ((struct multiboot2_tag_framebuffer *) frame_buffer_tag)->framebuffer_width;
	multibootInfo.framebuffer_height = ((struct multiboot2_tag_framebuffer *) frame_buffer_tag)->framebuffer_height;
	multibootInfo.framebuffer_bpp = ((struct multiboot2_tag_framebuffer *) frame_buffer_tag)->framebuffer_bpp;
	multibootInfo.framebuffer_type = ((struct multiboot2_tag_framebuffer *) frame_buffer_tag)->framebuffer_type;
	if (multibootInfo.framebuffer_type == 1) {
		multibootInfo.framebuffer_red_field_position = ((struct multiboot2_tag_framebuffer *) frame_buffer_tag)->framebuffer_red_field_position;
		multibootInfo.framebuffer_green_field_position = ((struct multiboot2_tag_framebuffer *) frame_buffer_tag)->framebuffer_green_field_position;
		multibootInfo.framebuffer_blue_field_position = ((struct multiboot2_tag_framebuffer *) frame_buffer_tag)->framebuffer_blue_field_position;
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