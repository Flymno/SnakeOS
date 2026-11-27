/* ---------------- Includes ---------------- */
#include <common/types.h>
#include <arch/boot/multiboot2.h>
#include <arch/boot/multiboot2_consts.h>
#include <drivers/console.h>

/* ---------------- Internal Constants ---------------- */
#define MAX_CALLBACKS 64

/* ---------------- Internal Types ---------------- */
typedef struct
{
	u32 type;
	u32 size;
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
status_t multiboot2_verify(u32 multiboot2Magic) {
	if (multiboot2Magic == MULTIBOOT2_MAGIC) { return SUCCESS;}
	return FAILURE;
}

void multiboot2_register_callback(multiboot2_tag_callback_t cb) {
	if (callbackCount < MAX_CALLBACKS) {
		callbacks[callbackCount++] = cb;
	} else {
		kprint_string("Too many multiboot2 callbacks!!\n");
	}
}

void multiboot2_init(u32 multiboot2InfoAddr)  {
	Multiboot2Tag_t *tag;
	
	for (tag = (Multiboot2Tag_t *)(uintptr_t)(multiboot2InfoAddr + 8); tag->type != TAG_END; tag = (Multiboot2Tag_t *) ((u8*) tag + ((tag->size + 7) & ~7))) {
		for (size_t i = 0; i < callbackCount; i++) {
			callbacks[i](tag);
		}
	};	
}