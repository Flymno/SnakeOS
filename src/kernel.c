#include <stddef.h>
#include <stdint.h>
#include "lib/string.h"
#include "drivers/serial/serial.h"
#include "arch/hwio/ports.h"
#include "arch/boot/multiboot2/multiboot2.h"
#include "arch/memory/pmm.h"
#include "arch/memory/memoryMap.h"
#include "arch/gdt/gdtInit.h"
#include "arch/graphics/framebuffer.h"

#define MAX_MEMMAP_ENTRIES 64

extern char _scode[], _end[];


void reboot()
{
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
}

void kernel_init(uint32_t multiboot2Magic, uint32_t multiboot2InfoAddr) 
{
	size_t kernelSize =  (size_t)_end - (size_t)_scode;

	gdt_init();
	serial_init();

	multiboot2_register_callback(framebuffer_multiboot2_init);
	multiboot2_register_callback(memorymap_multiboot2_init);

	serial_writestring("Welcome to SnakeOS!\n");
	serial_writestring("----------------------------------------\n\n");

	serial_writestring("Kernel base address: ");
	serial_writehex((size_t)_scode);
	serial_newline();
	serial_writestring("Kernel end address: ");
	serial_writehex((size_t)_end);
	serial_newline();
	serial_writestring("Kernel Size: ");
	serial_writehex(kernelSize);
	serial_writestring(" Bytes\n");
	serial_newline();

	serial_writestring("Multiboot magic: ");
	serial_writehex(multiboot2Magic);
	serial_newline();

	if (multiboot2_verify(multiboot2Magic) == 0) {
		serial_writestring("Multiboot 2 successfull\n");
		multiboot2_init(multiboot2InfoAddr);
	} else {
		serial_writestring("Multiboot 2 failed\n");
		return;
	}
	serial_newline();

	serial_writestring("FRAMEBUFFER:\n");

	serial_writestring("Framebuffer address: ");
	serial_writehex((uint32_t)framebuffer.addr);
	serial_newline();
	serial_writestring("Framebuffer pitch: ");
	serial_writehex(framebuffer.pitch);
	serial_newline();
	serial_writestring("Framebuffer width: ");
	serial_writehex(framebuffer.width);
	serial_newline();
	serial_writestring("Framebuffer height: ");
	serial_writehex(framebuffer.height);
	serial_newline();
	serial_writestring("Framebuffer bpp: ");
	serial_writehex(framebuffer.bpp);
	serial_newline();
	serial_writestring("Framebuffer type: ");
	serial_writehex(framebuffer.type);
	serial_newline();
	serial_newline();
	
	if ((uint32_t)framebuffer.addr > 0xffffffff) {
		reboot();
	}

	memorymap_dump();
	serial_newline();
	memorymap_dump_usable();
	serial_newline();

	pmm_init((uintptr_t)_end);

	uintptr_t allocatedPage;
	allocatedPage = palloc(16);
	if (allocatedPage != UINTPTR_MAX) {
		serial_writestring("Successfully requested pages. Returned Value: ");
		serial_writehex(allocatedPage);
		serial_newline();
	} else {
		serial_writestring("Uh oh, something went wrong. Returned Value: ");
		serial_writehex(allocatedPage);
		serial_newline();
	}

	if (pfree(allocatedPage, 16) == 0) {
		serial_writestring("Successfully freed pages \n");
	} else {
		serial_writestring("Uh oh, something went wrong. Failed to free pages \n");
	}

	allocatedPage = palloc(16);
	if (allocatedPage != UINTPTR_MAX) {
		serial_writestring("Successfully requested pages. Returned Value: ");
		serial_writehex(allocatedPage);
		serial_newline();
	} else {
		serial_writestring("Uh oh, something went wrong. Returned Value: ");
		serial_writehex(allocatedPage);
		serial_newline();
	}

	uintptr_t allocatedPage2;
	allocatedPage2 = palloc(16);
	if (allocatedPage2 != UINTPTR_MAX) {
		serial_writestring("Successfully requested pages. Returned Value: ");
		serial_writehex(allocatedPage2);
		serial_newline();
	} else {
		serial_writestring("Uh oh, something went wrong. Returned Value: ");
		serial_writehex(allocatedPage2);
		serial_newline();
	}


	uintptr_t allocatedPage3;
	allocatedPage3 = palloc(500);
	if (allocatedPage3 != UINTPTR_MAX) {
		serial_writestring("Successfully requested pages. Returned Value: ");
		serial_writehex(allocatedPage3);
		serial_newline();
	} else {
		serial_writestring("Uh oh, something went wrong. Returned Value: ");
		serial_writehex(allocatedPage3);
		serial_newline();
	}

	for (size_t y = 0; y < framebuffer.height; y++) {
		for (size_t x=0; x < framebuffer.width; x++) {
			uint8_t red = (x*255) / framebuffer.width;
			uint8_t green = (y*255) / framebuffer.height;
			uint8_t blue = ((framebuffer.height - y) * 255) / framebuffer.height;
			uint32_t color = (red << framebuffer.redPos) | (green << framebuffer.greenPos) | (blue << framebuffer.bluePos);

			size_t offset = y * framebuffer.pitch + x * framebuffer.bpp / 8;
			uint8_t* fb = (uint8_t*)(uintptr_t)framebuffer.addr;
			
			*((uint32_t*)(fb + offset)) = color;
		}
	}
}