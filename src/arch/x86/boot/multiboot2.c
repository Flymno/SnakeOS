/* ---------------- Includes ---------------- */
#include <stdint.h>
#include "arch/x86/boot/multiboot2.h"
#include "drivers/x86/serial/serial.h"
#include "arch/x86/memory/map/memoryMap.h"
#include "arch/x86/graphics/framebuffer.h"
#include "arch/x86/boot/multiboot2Consts.h"

/* ---------------- Internal Constants ---------------- */
#define MAX_CALLBACKS 64

/* ---------------- Internal Types ---------------- */
typedef struct
{
	uint32_t type;
	uint32_t size;
} Multiboot2Tag_t;

/* ---------------- Internal State ---------------- */
multiboot2_tag_callback_t callbacks[MAX_CALLBACKS];
size_t callbackCount;

/* ---------------- Public API Implementation ---------------- 
* uint8_t multiboot2_verify(uint32_t multiboot2Magic)
*	-Returns 0 if Multiboot2 was successful. Returns 1 otherwise
* void multiboot2_register_callback(multiboot2_tag_callback_t cb)
*	-Registers a callback for Multiboot2 initialisation
* void multiboot2_init(uint32_t multiboot2InfoAddr)
*	-Parses Multiboot2 info and calls registered callbacks
*/
uint8_t multiboot2_verify(uint32_t multiboot2Magic) {
	if (multiboot2Magic == MULTIBOOT2_MAGIC) { return 0;}
	return 1;
}

void multiboot2_register_callback(multiboot2_tag_callback_t cb) {
	if (callbackCount < MAX_CALLBACKS) {
		callbacks[callbackCount++] = cb;
	} else {
		serial_writestring("Too many Multiboot2 callbacks!\n");
	}
}

void multiboot2_init(uint32_t multiboot2InfoAddr)  {
	Multiboot2Tag_t *tag;
	
	for (tag = (Multiboot2Tag_t *) (multiboot2InfoAddr + 8); tag->type != TAG_END; tag = (Multiboot2Tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) {
		for (size_t i = 0; i < callbackCount; i++) {
			callbacks[i](tag);
		}
	};	
}