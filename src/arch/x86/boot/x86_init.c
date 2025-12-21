#include <stdint.h>
#include <stddef.h>
#include <arch/gdt/gdt.h>
#include <arch/drivers/serial/serial.h>
#include <hal/hal.h>
#include <drivers/console/console.h>
#include <arch/boot/multiboot2.h>

static HAL_t gHAL;
HAL_t* HAL = &gHAL;

void arch_kinit(uint32_t multiboot2Magic, uint32_t multiboot2InfoAddr) {
    gdt_init();

    HAL->console = x86_serial_get_console();
    HAL->console->init();

    if (multiboot2_verify(multiboot2Magic) == 0) {
		kprint_string("Multiboot 2 successfull\n");
		multiboot2_init(multiboot2InfoAddr);
	} else {
		kprint_string("Multiboot 2 failed\n");
		return;
	}

}