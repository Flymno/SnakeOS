#ifndef MEMORY_MAP_H_
#define MEMORY_MAP_H_

#include <stdint.h>
#include <stddef.h>
#include "arch/boot/multiboot2/multiboot2_consts.h"

#define MAX_MEMMAP_ENTRIES 64

typedef enum {
  RESERVED = 0,
  USABLE
} isUseable;

typedef struct
{
	uint64_t base;
	uint64_t len;
  uint8_t regionType;
} memoryRegion_t;

void memoryMap_multiboot2_init(const void* tag);

typedef void (*memoryMap_region_callback_t)(const memoryRegion_t *region);

void memorymap_foreach_raw(memoryMap_region_callback_t callback);
void memorymap_foreach_usable(memoryMap_region_callback_t callback);

uintptr_t memorymap_get_highest_address(void);

void dump_memMap(void);
void dump_usable_memory(void);

#endif