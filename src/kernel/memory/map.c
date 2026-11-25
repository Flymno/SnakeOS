/* ---------------- Includes ---------------- */
#include <common/types.h>
#include <memory/map.h>
#include <arch/drivers/serial/serial.h>

/* ---------------- Internal Constants ---------------- */
#define MAX_MEMMAP_ENTRIES 64

/* ---------------- Internal State ---------------- */
static MemoryRegion_t memoryMap[MAX_MEMMAP_ENTRIES];
size_t memoryMapSize = 0;

/* ---------------- Public API Implementation ----------------
* void memory_map_foreach_raw(memoryMap_region_callback_t callback)
*	-Iterates over all raw memory map entries and calls the callback
* void memory_map_foreach_usable(memoryMap_region_callback_t callback)
*	-Iterates over all usable memory map entries and calls the callback
* u64 memory_map_get_max_addr(void)
*	-Returns the highest address present in the memory map
*/

void memory_map_add_region(u64 baseAddr, u64 regionLen, size_t type) {
	if (memoryMapSize < MAX_MEMMAP_ENTRIES) {
			memoryMap[memoryMapSize].baseAddr = baseAddr;
			memoryMap[memoryMapSize].regionLen = regionLen;
			memoryMap[memoryMapSize].type = type;
			memoryMapSize++;
	}
}

void memory_map_foreach_raw(memory_map_region_callback_t callback) {
	for (size_t i = 0; i < memoryMapSize; i++) {
		callback(&memoryMap[i]);
	}
}

void memory_map_foreach_usable(memory_map_region_callback_t callback) {
	for (size_t i = 0; i < memoryMapSize; i++) {
		if (memoryMap[i].type == MEMORY_REGION_USABLE) callback(&memoryMap[i]);
	}
}

u64 memory_map_get_max_addr(void) {
	if (memoryMapSize == 0) return 0;

	u64 highest = memoryMap[0].baseAddr + memoryMap[0].regionLen;
	for (size_t i = 1; i < memoryMapSize; i++) {
		u64 end = memoryMap[i].baseAddr + memoryMap[i].regionLen;
		if (end > highest) highest = end;
	}
	return highest;
}

u64 memory_map_get_max_addr_usable(void) {
	if (memoryMapSize == 0) return 0;

	u64 highest = memoryMap[0].baseAddr + memoryMap[0].regionLen;
	for (size_t i = 1; i < memoryMapSize; i++) {
		if (memoryMap[i].type == MEMORY_REGION_USABLE) {
			u64 end = memoryMap[i].baseAddr + memoryMap[i].regionLen;
			if (end > highest) highest = end;
		}
	}
	return highest;
}

u64 memory_map_get_toal_mem(void) {
	if (memoryMapSize == 0) return 0;
	u64 sum = 0;
	for (size_t i = 1; i < memoryMapSize; i++) {
		if (memoryMap[i].type == MEMORY_REGION_USABLE) {
			sum += memoryMap[i].regionLen;
		}
	}
	return sum;
}