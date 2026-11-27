#include <common/types.h>
#include <hal/hal.h>
#include <lib/stdio.h>
#include <memory/map.h>

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

void kmain(void) {
    printf("\n---------- Welcome To SnakeOS ----------\n");

    memory_map_dump();
    printf("\n");
    hal_device_dump();
}