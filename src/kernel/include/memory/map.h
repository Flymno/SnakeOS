#ifndef MAP_H_
#define MAP_H_

#include <common/types.h>

#define MEMORY_REGION_USABLE 1
#define MEMORY_REGION_RESERVED 0

typedef struct 
{
    u64 baseAddr;
    u64 regionLen;
    size_t type;
} MemoryRegion_t;

typedef void (*memory_map_region_callback_t)(const MemoryRegion_t *region);


void memory_map_add_region(u64 baseAddr, u64 regionLen, size_t type);
void memory_map_foreach_raw(memory_map_region_callback_t callback);
void memory_map_foreach_usable(memory_map_region_callback_t callback);
u64 memory_map_get_max_addr(void);
u64 memory_map_get_max_addr_usable(void);

#endif