#include <stdint.h>
#include <stddef.h>
#include "arch/memory/memoryMap.h"
#include "drivers/serial/serial.h"

#define MAX_MEMMAP_ENTRIES 64

memoryRegion memoryMap[MAX_MEMMAP_ENTRIES];
size_t memoryMap_Size = 0;

usableMemoryRegion usableMemoryMap[MAX_MEMMAP_ENTRIES];
size_t usableMemoryMap_Size = 0;

void add_mmap_entry(memoryRegion *mmapEntry) {
	memoryMap[memoryMap_Size] = *mmapEntry;
	memoryMap_Size++;
}

void filter_usable_memory() {
	for (size_t i = 0; i < memoryMap_Size; i++) {
		if (memoryMap[i].type == 1) {
			usableMemoryMap[usableMemoryMap_Size].base = memoryMap[i].addr;
			usableMemoryMap[usableMemoryMap_Size].len = memoryMap[i].len;
			usableMemoryMap_Size++;
		}
	}
}

void dump_memMap() {
	serial_writestring("Memory Map Location: ");
	serial_writehex((uint32_t)memoryMap);
	serial_newline();
	serial_writestring("Memory Map Entries: ");
	serial_newline();
	for (size_t i = 0; i < memoryMap_Size; i++) {
		serial_writestring("Base High: ");
		serial_writehex((uint32_t)(memoryMap[i].addr >> 32));
		serial_writestring("    Base Low: ");
		serial_writehex((uint32_t)memoryMap[i].addr);
		serial_newline();
		serial_writestring("Length High: ");
		serial_writehex((uint32_t)(memoryMap[i].len >> 32));
		serial_writestring("    Length Low: ");
		serial_writehex((uint32_t)memoryMap[i].len);
		serial_newline();
		serial_writestring("Type: ");
		serial_writehex((uint32_t)memoryMap[i].type);
		serial_newline();
	}
}

void dump_usable_memory() {
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