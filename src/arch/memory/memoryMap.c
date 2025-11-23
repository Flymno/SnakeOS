#include <stdint.h>
#include <stddef.h>
#include "arch/memory/memoryMap.h"
#include "arch/boot/multiboot2/multiboot2_consts.h"
#include "drivers/serial/serial.h"

#define MAX_MEMMAP_ENTRIES 64

typedef struct 
{
  uint64_t addr;
  uint64_t len;
  uint32_t type;
  uint32_t zero;
} rawMemoryRegion;

struct multiboot2_tag_mmap
{
  uint32_t type;
  uint32_t size;
  uint32_t entry_size;
  uint32_t entry_version;
  rawMemoryRegion entries[];  
};

memoryRegion_t memoryMap[MAX_MEMMAP_ENTRIES];
size_t memoryMap_Size = 0;

memoryRegion_t usableMemoryMap[MAX_MEMMAP_ENTRIES];
size_t usableMemoryMap_Size = 0;

void memoryMap_multiboot2_init(const void* tag) {
	const struct multiboot2_tag_mmap* mmap_tag = tag;

	if(mmap_tag->type != TAG_MMAP) {
		return;
	}

	size_t entryCount = (mmap_tag->size - sizeof(*mmap_tag)) / mmap_tag->entry_size;
	const rawMemoryRegion* entry = mmap_tag->entries;

	for (size_t i = 0; i < entryCount; i++) {
		if (memoryMap_Size < MAX_MEMMAP_ENTRIES) {
			memoryMap[memoryMap_Size].base = entry->addr;
			memoryMap[memoryMap_Size].len = entry->len;
			memoryMap[memoryMap_Size].regionType = entry->type;
		}

    	entry = (const rawMemoryRegion*)((uintptr_t)entry + mmap_tag->entry_size);
	}

	filter_usable_memory();
}

void filter_usable_memory() {
	for (size_t i = 0; i < memoryMap_Size; i++) {
		if (memoryMap[i].regionType == MEMORY_AVAILABLE) {
			usableMemoryMap[usableMemoryMap_Size].base = memoryMap[i].base;
			usableMemoryMap[usableMemoryMap_Size].len = memoryMap[i].len;
			usableMemoryMap_Size++;
		}
	}
}

void memorymap_foreach_raw(memoryMap_region_callback_t callback) {
	for (size_t i = 0; i < memoryMap_Size; i++) {
		callback(&memoryMap[i]);
	}
}

void memorymap_foreach_usable(memoryMap_region_callback_t callback) {
	for (size_t i = 0; i < usableMemoryMap_Size; i++) {
		callback(&usableMemoryMap[i]);
	}
}

uintptr_t memorymap_get_highest_address(void) {
	if (memoryMap_Size == 0) return 0;

	uintptr_t highest = memoryMap[0].base + memoryMap[0].len;
	for (size_t i = 1; i < memoryMap_Size; i++) {
		uintptr_t end = memoryMap[i].base + memoryMap[i].len;
		if (end > highest) highest = end;
	}
	return highest;
}

void dump_memMap(void) {
	serial_writestring("Memory Map Location: ");
	serial_writehex((uint32_t)memoryMap);
	serial_newline();
	serial_writestring("Memory Map Entries: ");
	serial_newline();
	for (size_t i = 0; i < memoryMap_Size; i++) {
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

void dump_usable_memory(void) {
	serial_writestring("Usable Memory Map Location: ");
	serial_writehex((uint32_t)usableMemoryMap);
	serial_newline();
	serial_writestring("Usable Memory Map Entries: ");
	serial_newline();
	for (size_t i = 0; i < usableMemoryMap_Size; i++) {
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