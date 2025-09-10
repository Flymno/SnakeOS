#include <stddef.h>
#include <stdint.h>
#include "lib/string.h"
#include "drivers/serial/serial.h"
#include "arch/hwio/ports.h"
#include "arch/boot/multiboot2/multiboot2.h"
#include "arch/memory/pmm.h"
#include "arch/memory/memoryMap.h"
#include "arch/gdt/gdtInit.h"

#define MAX_MEMMAP_ENTRIES 64

extern char _scode[], _end[];


void reboot()
{
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
}

void kernel_init(uint32_t multiboot2_magic, uint32_t multiboot2_info_addr) 
{
	size_t kernelSize =  (size_t)_end - (size_t)_scode;

	/* init_pics(0x20, 0x28); */

	init_gdt();

	init_serial();

	serial_writestring("Welcome to MyOs!\n");
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
	serial_writehex(multiboot2_magic);
	serial_newline();

	if (verify_multiboot(multiboot2_magic) == 0) {
		serial_writestring("Multiboot 2 successfull\n");
		multiboot2_init(multiboot2_info_addr);
	} else {
		serial_writestring("Multiboot 2 failed\n");
		return;
	}
	serial_newline();

	serial_writestring("FRAMEBUFFER:\n");

	serial_writestring("Framebuffer address: ");
	serial_writehex((uint32_t)multibootInfo.framebuffer_addr);
	serial_newline();
	serial_writestring("Framebuffer pitch: ");
	serial_writehex(multibootInfo.framebuffer_pitch);
	serial_newline();
	serial_writestring("Framebuffer width: ");
	serial_writehex(multibootInfo.framebuffer_width);
	serial_newline();
	serial_writestring("Framebuffer height: ");
	serial_writehex(multibootInfo.framebuffer_height);
	serial_newline();
	serial_writestring("Framebuffer bpp: ");
	serial_writehex(multibootInfo.framebuffer_bpp);
	serial_newline();
	serial_writestring("Framebuffer type: ");
	serial_writehex(multibootInfo.framebuffer_type);
	serial_newline();
	serial_newline();
	
	if ((uint32_t)multibootInfo.framebuffer_addr > 0xffffffff) {
		reboot();
	}

	filter_usable_memory();
	dump_memMap();
	serial_newline();
	dump_usable_memory();
	serial_newline();

	init_bitmap_allocator((uintptr_t)_end);

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

	if (pfree(allocatedPage, 16)) {
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

	for (size_t y = 0; y < multibootInfo.framebuffer_height; y++) {
		for (size_t x=0; x < multibootInfo.framebuffer_width; x++) {
			uint8_t red = (x*255) / multibootInfo.framebuffer_width;
			uint8_t green = (y*255) / multibootInfo.framebuffer_height;
			uint8_t blue = ((multibootInfo.framebuffer_height - y) * 255) / multibootInfo.framebuffer_height;
			uint32_t color = (red << multibootInfo.framebuffer_red_field_position) | (green << multibootInfo.framebuffer_green_field_position) | (blue << multibootInfo.framebuffer_blue_field_position);

			size_t offset = y * multibootInfo.framebuffer_pitch + x * multibootInfo.framebuffer_bpp / 8;
			uint8_t* fb = (uint8_t*)multibootInfo.framebuffer_addr;
			
			*((uint32_t*)(fb + offset)) = color;
		}
	}

	
}