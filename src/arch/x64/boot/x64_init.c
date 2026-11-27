#include <common/types.h>
#include <arch/drivers/serial/serial.h>
#include <hal/hal.h>
#include <drivers/console.h>
#include <arch/boot/multiboot2.h>
#include <arch/boot/mmap_parse.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <memory/pmm.h>
#include <memory/map.h>
#include <memory/heap.h>

extern char _physStart[], _physEnd[], _vStart[], _vEnd[];

static void memory_map_dump_callback(const MemoryRegion_t* region) {
    const char* status = (region->type == MEMORY_REGION_USABLE) ? "USABLE" : "RESERVED";
    
    printf("%#08x%08x | %#08x%08x | %s\n", 
           (u32)(region->baseAddr >> 32), (u32)region->baseAddr,
           (u32)(region->regionLen >> 32), (u32)region->regionLen,
           status);
}

static void memory_map_dump(void) {
    printf("\n--- System Memory Map ---\n");
    printf("Base Address         | Length               | Status\n");
    printf("------------------------------------------------------------\n");
    
    memory_map_foreach_raw(memory_map_dump_callback);
    
    u64 max_addr = memory_map_get_max_addr();
    printf("------------------------------------------------------------\n");
    printf("Highest Addressable: %#08x%08x (%d MB)\n", 
           (u32)(max_addr >> 32), (u32)max_addr, 
           (u32)(max_addr / 1024 / 1024));
}

static bool hal_device_dump_callback(const HALDevice_t* device) {
    CmdIdentResponse_t ident = {0};
    if (device->control(CMD_IDENT, (hal_arg_t)0, (hal_arg_t)&ident) == FAILURE) {
        return false;
    }
    printf("Found Device: %s\n", ident.name);
    return false;
}

static void hal_device_dump(void) {
    hal_foreach_device(hal_device_dump_callback);
}

void arch_kinit(u32 multiboot2Magic, u32 multiboot2InfoAddr) {
    bool mb_valid = false;
    if (multiboot2_verify(multiboot2Magic) == SUCCESS) mb_valid = true;

	multiboot2_register_callback(memory_map_multiboot2_init);

    if (mb_valid) multiboot2_init(multiboot2InfoAddr);

	pmm_init((uintptr_t)_vEnd);

    hal_register_device(serial_hal_registration);
    HAL_init();

    printf("\n---------- Welcome To SnakeOS ----------\n");
	printf("> Kernel Start Address: %#08x%08x\n", (u32)((uintptr_t)_vStart >> 32), (u32)(_vStart));
	printf("> Initialising Multiboot 2...\n");

    if (mb_valid) {
        printf("> Multiboot 2 successful! Magic: %#08x\n", multiboot2Magic);
    } else {
        printf("> FATAL! Multiboot 2 failed! Magic: %#08x\n", multiboot2Magic);
		return;
    }

	memory_map_dump();
    hal_device_dump();
}
