/* ---------------- Includes ---------------- */
#include <stdint.h>
#include <stddef.h>
#include "arch/x86/graphics/framebuffer.h"
#include "arch/x86/boot/multiboot2_consts.h"
#include "drivers/x86/serial/serial.h"

/* ---------------- Internal Types ---------------- */
typedef struct 
{
  uint32_t type;
  uint32_t size;

  uint64_t framebufferAddr;
  uint32_t framebufferPitch;
  uint32_t framebufferWidth;
  uint32_t framebufferHeight;
  uint8_t framebufferBpp;
  uint8_t framebufferType;
  uint16_t reserved;

  struct {
	uint8_t framebufferRedFieldPosition;
    uint8_t framebufferRedMaskSize;
    uint8_t framebufferGreenFieldPosition;
    uint8_t framebufferGreenMaskSize;
    uint8_t framebufferBlueFieldPosition;
    uint8_t framebufferBlueMaskSize;
  };
} Multiboot2TagFramebuffer_t;

/* ---------------- Internal State ---------------- */
Framebuffer_t framebuffer = {0};

/* ---------------- Public API Implementation ---------------- 
* void framebuffer_multiboot2_init(const void* tag)
*	-Initialises framebuffer from a Multiboot2 framebuffer tag
*/
void framebuffer_multiboot2_init(const void* tag) {
	const Multiboot2TagFramebuffer_t* framebufferTag = tag;

	if (framebufferTag->type != TAG_FRAMEBUFFER) {
		serial_writestring("Not a framebuffer tag! Tag: ");
		serial_writehex(framebufferTag->type);
		serial_newline();
		return;
	}

	serial_writestring("Framebuffer tag found!\n");
	serial_writestring("	Type: ");
	serial_writehex(framebufferTag->type);
	serial_newline();
	serial_writestring("	Size: ");
	serial_writehex(framebufferTag->size);
	serial_newline();

	framebuffer.addr = framebufferTag->framebufferAddr;
	framebuffer.pitch = framebufferTag->framebufferPitch;
	framebuffer.width = framebufferTag-> framebufferWidth;
	framebuffer.height = framebufferTag->framebufferHeight;
	framebuffer.bpp = framebufferTag->framebufferBpp;
	framebuffer.type = framebufferTag->framebufferType;

	if (framebuffer.type == FRAMEBUFFER_RGB) {
		framebuffer.redPos = framebufferTag->framebufferRedFieldPosition;
		framebuffer.greenPos = framebufferTag->framebufferGreenFieldPosition;
		framebuffer.bluePos = framebufferTag->framebufferBlueFieldPosition;
	} else {
		framebuffer.redPos = -1;
		framebuffer.greenPos = -1;
		framebuffer.bluePos = -1;
	}
}