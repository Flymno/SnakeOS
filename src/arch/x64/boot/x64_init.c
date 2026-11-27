#include <common/types.h>
#include <arch/drivers/serial.h>
#include <hal/hal.h>
#include <arch/boot/multiboot2.h>
#include <arch/boot/mmap_parse.h>
#include <memory/pmm.h>
#include <memory/map.h>
#include <kernel.h>

extern char _physStart[], _physEnd[], _vStart[], _vEnd[];

void arch_kinit(u32 multiboot2Magic, u32 multiboot2InfoAddr) {
    if (multiboot2_verify(multiboot2Magic) == SUCCESS) {
	    multiboot2_register_callback(memory_map_multiboot2_init);
        multiboot2_init(multiboot2InfoAddr);
    }

	pmm_init((uintptr_t)_vEnd);

    hal_register_device(serial_hal_registration);
    HAL_init();

    kmain();
}
