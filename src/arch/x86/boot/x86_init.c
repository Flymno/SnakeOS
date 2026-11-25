#include <common/types.h>
#include <arch/gdt/gdt.h>
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

static HAL_t gHAL;
HAL_t* HAL = &gHAL;

static void dump_callback(const MemoryRegion_t* region) {
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
    
    memory_map_foreach_raw(dump_callback);
    
    u64 max_addr = memory_map_get_max_addr();
    printf("------------------------------------------------------------\n");
    printf("Highest Addressable: %#08x%08x (%d MB)\n", 
           (u32)(max_addr >> 32), (u32)max_addr, 
           (u32)(max_addr / 1024 / 1024));
}

void arch_kinit(u32 multiboot2Magic, u32 multiboot2InfoAddr) {
    // gdt_init();

    HAL->console = x86_serial_get_console();
    HAL->console->init();

	printf("\n---------- Welcome To SnakeOS ----------\n");
	printf("> Kernel Start Address: %#08x\n", (u32)_vStart);
	printf("> Initialising Multiboot 2...\n");

	multiboot2_register_callback(memory_map_multiboot2_init);

    if (multiboot2_verify(multiboot2Magic) == 0) {
		printf("> Multiboot 2 successful! Magic: %#08x\n", multiboot2Magic);
		multiboot2_init(multiboot2InfoAddr);
	} else {
		printf("> FATAL! Multiboot 2 failed! Magic: %#08x\n", multiboot2Magic);
		return;
	}

	pmm_init((uintptr_t)_vEnd);

    char* stringTest = kcalloc(sizeof(char), 15);
    strcpy(stringTest, "Hello, World!\n");
    printf("%s", stringTest);
    kfree(stringTest);
    printf("%s", stringTest);

	memory_map_dump();
}
