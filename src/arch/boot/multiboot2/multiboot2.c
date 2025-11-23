#include <stdint.h>
#include "arch/boot/multiboot2/multiboot2.h"
#include "drivers/serial/serial.h"
#include "arch/memory/memoryMap.h"
#include "arch/graphics/framebuffer.h"
#include "arch/boot/multiboot2/multiboot2_consts.h"

#define MAX_CALLBACKS 64

typedef struct
{
	uint32_t type;
	uint32_t size;
} multiboot2_tag_t;

int verify_multiboot(uint32_t multiboot2_magic) {
	if (multiboot2_magic == MULTIBOOT2_MAGIC) { return 0;}
	return 1;
}

multiboot2_tag_callback_t callbacks[MAX_CALLBACKS];
size_t callback_count;

void multiboot2_register_callback(multiboot2_tag_callback_t cb) {
	callbacks[callback_count++] = cb;
}

void multiboot2_parse(uint32_t multiboot2_info_addr)  {
	multiboot2_tag_t *tag;
	
	for (tag = (struct multiboot2_tag *) (multiboot2_info_addr + 8); tag->type != TAG_END; tag = (struct multiboot2_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) {
		for (size_t i = 0; i < callback_count; i++) {
			callbacks[i](tag);
		}
	};	
}