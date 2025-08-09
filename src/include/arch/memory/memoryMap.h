#ifndef MEMORY_MAP_H_
#define MEMORY_MAP_H_

#include <stdint.h>
#include <stddef.h>

#define MAX_MEMMAP_ENTRIES 64

typedef struct 
{
  uint64_t addr;
  uint64_t len;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
  uint32_t type;
  uint32_t zero;
} memoryRegion;

typedef struct
{
	uint64_t base;
	uint64_t len;
} usableMemoryRegion;

void add_mmap_entry(memoryRegion *mmapEntry);

void filter_usable_memory();

void dump_memMap();

void dump_usable_memory();

extern memoryRegion memoryMap[MAX_MEMMAP_ENTRIES];
extern size_t memoryMap_Size;

extern usableMemoryRegion usableMemoryMap[MAX_MEMMAP_ENTRIES];
extern size_t usableMemoryMap_Size;

#endif