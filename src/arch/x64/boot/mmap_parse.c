#include <common/types.h>
#include <arch/boot/multiboot2_consts.h>
#include <memory/map.h>

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

/* ---------------- Public API Implementation ----------------
* void memoryMap_multiboot2_init(const void* tag)
*	-Initialises memory map from a Multiboot2 memory map tag
*/

void memory_map_multiboot2_init(const void* tag) {
	const Multiboot2TagMMap_t* mmap_tag = tag;

	if(mmap_tag->type != TAG_MMAP) {
		return;
	}

	const u8* end = (const u8*)mmap_tag + mmap_tag->size;
	const u8* entry_ptr = (const u8*)mmap_tag->entries;

	while (entry_ptr < end) {
		const RawMemoryRegion_t* entry = (const RawMemoryRegion_t*)entry_ptr;

        size_t type = (entry->type == MEMORY_AVAILABLE) ? MEMORY_REGION_USABLE : MEMORY_REGION_RESERVED;

        memory_map_add_region(entry->addr, entry->len, type);

		entry_ptr += mmap_tag->entrySize;
	}
}