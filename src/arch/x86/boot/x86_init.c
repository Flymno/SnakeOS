#include <common/types.h>
#include <arch/gdt/gdt.h>
#include <arch/drivers/serial/serial.h>
#include <hal/hal.h>
#include <drivers/console.h>
#include <arch/boot/multiboot2.h>
#include <lib/stdio.h>

static HAL_t gHAL;
HAL_t* HAL = &gHAL;

void arch_kinit(u32 multiboot2Magic, u32 multiboot2InfoAddr) {
    gdt_init();

    HAL->console = x86_serial_get_console();
    HAL->console->init();

	printf("Test hex with alternate flag and 8 width flag: %#08x \n", 1532);

    if (multiboot2_verify(multiboot2Magic) == 0) {
		kprint_string("Multiboot 2 successfull\n");
		multiboot2_init(multiboot2InfoAddr);
	} else {
		kprint_string("Multiboot 2 failed\n");
		return;
	}

}