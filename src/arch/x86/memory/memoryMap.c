/* ---------------- Includes ---------------- */
#include <common/types.h>
#include <arch/memory/memoryMap.h>
#include <arch/boot/multiboot2Consts.h>
#include <arch/drivers/serial/serial.h>

/* ---------------- Internal Constants ---------------- */
#define MAX_MEMMAP_ENTRIES 64

/* ---------------- Internal Types ---------------- */
typedef struct 
{
  u64 addr;
  u64 len;
  u32 type;
  u32 zero;
} RawMemoryRegion_t;

typedef struct
{
  u32 type;
  u32 size;
  u32 entrySize;
  u32 entryVersion;
  RawMemoryRegion_t entries[];  
} Multiboot2TagMMap_t;

/* ---------------- Internal State ---------------- */
static MemoryRegion_t memoryMap[MAX_MEMMAP_ENTRIES];
size_t memoryMapSize = 0;

static MemoryRegion_t usableMemoryMap[MAX_MEMMAP_ENTRIES];
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
		return;
	}

	const u8* end = (const u8*)mmap_tag + mmap_tag->size;
	const u8* entry_ptr = (const u8*)mmap_tag->entries;

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

u64 memorymap_get_max_addr(void) {
	if (memoryMapSize == 0) return 0;

	u64 highest = memoryMap[0].base + memoryMap[0].len;
	for (size_t i = 1; i < memoryMapSize; i++) {
		u64 end = memoryMap[i].base + memoryMap[i].len;
		if (end > highest) highest = end;
	}
	return highest;
}

// void memorymap_dump(void) {
// 	serial_writestring("Memory Map Location: ");
// 	serial_writehex((u32)memoryMap);
// 	serial_newline();
// 	serial_writestring("Memory Map Entries: ");
// 	serial_newline();
// 	for (size_t i = 0; i < memoryMapSize; i++) {
// 		serial_writestring("Base High: ");
// 		serial_writehex((u32)(memoryMap[i].base >> 32));
// 		serial_writestring("    Base Low: ");
// 		serial_writehex((u32)memoryMap[i].base);
// 		serial_newline();
// 		serial_writestring("Length High: ");
// 		serial_writehex((u32)(memoryMap[i].len >> 32));
// 		serial_writestring("    Length Low: ");
// 		serial_writehex((u32)memoryMap[i].len);
// 		serial_newline();
// 		serial_writestring("Type: ");
// 		serial_writehex((u32)memoryMap[i].regionType);
// 		serial_newline();
// 	}
// }

// void memorymap_dump_usable(void) {
// 	serial_writestring("Usable Memory Map Location: ");
// 	serial_writehex((u32)usableMemoryMap);
// 	serial_newline();
// 	serial_writestring("Usable Memory Map Entries: ");
// 	serial_newline();
// 	for (size_t i = 0; i < usableMemoryMapSize; i++) {
// 		serial_writestring("Base High: ");
// 		serial_writehex((u32)(usableMemoryMap[i].base >> 32));
// 		serial_writestring("    Base Low: ");
// 		serial_writehex((u32)usableMemoryMap[i].base);
// 		serial_newline();
// 		serial_writestring("Length High: ");
// 		serial_writehex((u32)(usableMemoryMap[i].len >> 32));
// 		serial_writestring("    Length Low: ");
// 		serial_writehex((u32)usableMemoryMap[i].len);
// 		serial_newline();
// 	}
// }