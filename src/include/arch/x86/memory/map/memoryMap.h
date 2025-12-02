#ifndef MEMORY_MAP_H_
#define MEMORY_MAP_H_

/* ---------------- Includes ---------------- */
#include <stdint.h>
#include <stddef.h>
#include "arch/x86/boot/multiboot2_consts.h"

/* ---------------- Public Constants ---------------- */
#define MAX_MEMMAP_ENTRIES 64

/* ---------------- Public Types ---------------- */
typedef struct
{
	uint64_t base;
	uint64_t len;
  uint8_t regionType;
} MemoryRegion_t;

typedef void (*memoryMap_region_callback_t)(const MemoryRegion_t *region);

/* ---------------- Public API Prototypes ----------------
* void memoryMap_multiboot2_init(const void* tag)
*	-Initialises memory map from a Multiboot2 memory map tag
* void memorymap_foreach_raw(memoryMap_region_callback_t callback)
*	-Iterates over all raw memory map entries and calls the callback
* void memorymap_foreach_usable(memoryMap_region_callback_t callback)
*	-Iterates over all usable memory map entries and calls the callback
* uint64_t memorymap_get_max_addr(void)
*	-Returns the highest address present in the memory map
* void memorymap_dump(void)
*	-Prints all memory map entries for debugging
* void memorymap_dump_usable(void)
*	-Prints all usable memory map entries for debugging
*/
void memorymap_multiboot2_init(const void* tag);
void memorymap_foreach_raw(memoryMap_region_callback_t callback);
void memorymap_foreach_usable(memoryMap_region_callback_t callback);
uint64_t memorymap_get_max_addr(void);
void memorymap_dump(void);
void memorymap_dump_usable(void);

#endif