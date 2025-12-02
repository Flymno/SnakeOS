/* ---------------- Includes ---------------- */
#include <stdint.h>
#include <stddef.h>
#include "arch/x86/memory/map/memoryMap.h"
#include "arch/x86/boot/multiboot2_consts.h"
#include "drivers/x86/serial/serial.h"

/* ---------------- Internal Constants ---------------- */
#define MAX_MEMMAP_ENTRIES 64

/* ---------------- Internal Types ---------------- */
typedef struct 
{
  uint64_t addr;
  uint64_t len;
  uint32_t type;
  uint32_t zero;
} RawMemoryRegion_t;

typedef struct
{
  uint32_t type;
  uint32_t size;
  uint32_t entrySize;
  uint32_t entryVersion;
  RawMemoryRegion_t entries[];  
} Multiboot2TagMMap_t;

/* ---------------- Internal State ---------------- */
MemoryRegion_t memoryMap[MAX_MEMMAP_ENTRIES];
size_t memoryMapSize = 0;

MemoryRegion_t usableMemoryMap[MAX_MEMMAP_ENTRIES];
size_t usableMemoryMapSize = 0;

/* ---------------- Internal Helper Prototypes ---------------- */
static void filter_usable_memory(void);

/* ---------------- Internal Helper Implementation ----------------
* static void filter_usable_memory(void)
*	-Populates the usable memory map with only the usable entries from the memory map
*/
static void filter_usable_memory(void) {
	for (size_t i = 0; i < memoryMapSize; i++) {
		if (memoryMap[i].regionType == MEMORY_AVAILABLE) {
			usableMemoryMap[usableMemoryMapSize] = memoryMap[i];
			usableMemoryMapSize++;
		}
	}
}

/* ---------------- Public API Implementation ----------------
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
void memorymap_multiboot2_init(const void* tag) {
	const Multiboot2TagMMap_t* mmap_tag = tag;

	if(mmap_tag->type != TAG_MMAP) {
		serial_writestring("Not a memory map tag! Tag: ");
		serial_writehex(mmap_tag->type);
		serial_newline();
		return;
	}

	serial_writestring("Memory map tag found!\n");
	serial_writestring("	Type: ");
	serial_writehex(mmap_tag->type);
	serial_newline();
	serial_writestring("	Size: ");
	serial_writehex(mmap_tag->size);
	serial_newline();

	const uint8_t* end = (const uint8_t*)mmap_tag + mmap_tag->size;
	const uint8_t* entry_ptr = (const uint8_t*)mmap_tag->entries;

	while (entry_ptr < end) {
		const RawMemoryRegion_t* entry = (const RawMemoryRegion_t*)entry_ptr;

		if (memoryMapSize < MAX_MEMMAP_ENTRIES) {
			memoryMap[memoryMapSize].base = entry->addr;
			memoryMap[memoryMapSize].len = entry->len;
			memoryMap[memoryMapSize].regionType = entry->type;
			memoryMapSize++;
		}

		entry_ptr += mmap_tag->entrySize;
	}

	filter_usable_memory();
}

void memorymap_foreach_raw(memoryMap_region_callback_t callback) {
	for (size_t i = 0; i < memoryMapSize; i++) {
		callback(&memoryMap[i]);
	}
}

void memorymap_foreach_usable(memoryMap_region_callback_t callback) {
	for (size_t i = 0; i < usableMemoryMapSize; i++) {
		callback(&usableMemoryMap[i]);
	}
}

uint64_t memorymap_get_max_addr(void) {
	if (memoryMapSize == 0) return 0;

	uint64_t highest = memoryMap[0].base + memoryMap[0].len;
	for (size_t i = 1; i < memoryMapSize; i++) {
		uint64_t end = memoryMap[i].base + memoryMap[i].len;
		if (end > highest) highest = end;
	}
	return highest;
}

void memorymap_dump(void) {
	serial_writestring("Memory Map Location: ");
	serial_writehex((uint32_t)memoryMap);
	serial_newline();
	serial_writestring("Memory Map Entries: ");
	serial_newline();
	for (size_t i = 0; i < memoryMapSize; i++) {
		serial_writestring("Base High: ");
		serial_writehex((uint32_t)(memoryMap[i].base >> 32));
		serial_writestring("    Base Low: ");
		serial_writehex((uint32_t)memoryMap[i].base);
		serial_newline();
		serial_writestring("Length High: ");
		serial_writehex((uint32_t)(memoryMap[i].len >> 32));
		serial_writestring("    Length Low: ");
		serial_writehex((uint32_t)memoryMap[i].len);
		serial_newline();
		serial_writestring("Type: ");
		serial_writehex((uint32_t)memoryMap[i].regionType);
		serial_newline();
	}
}

void memorymap_dump_usable(void) {
	serial_writestring("Usable Memory Map Location: ");
	serial_writehex((uint32_t)usableMemoryMap);
	serial_newline();
	serial_writestring("Usable Memory Map Entries: ");
	serial_newline();
	for (size_t i = 0; i < usableMemoryMapSize; i++) {
		serial_writestring("Base High: ");
		serial_writehex((uint32_t)(usableMemoryMap[i].base >> 32));
		serial_writestring("    Base Low: ");
		serial_writehex((uint32_t)usableMemoryMap[i].base);
		serial_newline();
		serial_writestring("Length High: ");
		serial_writehex((uint32_t)(usableMemoryMap[i].len >> 32));
		serial_writestring("    Length Low: ");
		serial_writehex((uint32_t)usableMemoryMap[i].len);
		serial_newline();
	}
}