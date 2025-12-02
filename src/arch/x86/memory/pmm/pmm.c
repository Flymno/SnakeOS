/* ---------------- Includes ---------------- */
#include <stdint.h>
#include <stddef.h>
#include "arch/memory/pmm/pmm.h"
#include "arch/boot/multiboot2/multiboot2.h"
#include "arch/memory/memoryMap.h"
#include "drivers/serial/serial.h"
#include "lib/string.h"

/* ---------------- Internal Constants ---------------- */
#define PAGE_SIZE 4096
#define BITS_PER_ROW 32

/* ---------------- Extern Symbols ---------------- */
extern char _physStart[], _physEnd[];

/* ---------------- Internal Types ---------------- */
typedef struct
{
	uint32_t* bitmap;
	size_t bitmapLength;
	uint64_t totalPages;
	uint64_t lastAllocatedIndex;
} PMMState_t;

typedef struct 
{
	uint32_t rowIndex;
	uint8_t bitIndex;
} BitmapLocation_t;

/* ---------------- Internal State ---------------- */
static PMMState_t pmm;

/* ---------------- Internal Helper Prototypes ---------------- */
static BitmapLocation_t get_location(uint64_t pageIndex);
static uint64_t page_to_address(uint64_t pageIndex);
static uint64_t address_to_page(uint64_t address);
static uint8_t bitmap_test(uint64_t pageIndex);
static uint8_t bitmap_set(uint64_t pageIndex);
static uint8_t bitmap_clear(uint64_t pageIndex);
static void bitmap_clear_region_callback(const MemoryRegion_t* region);
static uint8_t bitmap_find_run(uint64_t start, uint64_t end, uint64_t pageCount, uint64_t* outIndex);
static void kernel_allocate(void);
static void bitmap_allocate(void);

/* ---------------- Internal Helper Implementation ----------------
* static BitmapLocation_t get_location(uint64_t pageIndex)
*   -Returns the location of a page's bit in the bitmap (row and column)
* static uint64_t page_to_address(uint64_t pageIndex)
*   -Converts a page index to a physical address
* static uint64_t address_to_page(uint64_t address)
*   -Converts a physical address to a page index
* static uint8_t bitmap_test(uint64_t pageIndex)
*   -Checks if a page is allocated. Returns 0 if free, 1 if allocated
* static uint8_t bitmap_set(uint64_t pageIndex)
*   -Marks a page as allocated. Returns 0 on success, 1 on failure
* static uint8_t bitmap_clear(uint64_t pageIndex)
*   -Marks a page as free. Returns 0 on success, 1 on failure
* static void bitmap_clear_region_callback(const MemoryRegion_t* region)
*   -Clears all pages in the bitmap corresponding to the given memory region
* static uint8_t bitmap_find_run(uint64_t start, uint64_t end, uint64_t pageCount, uint64_t* outIndex)
*   -Finds a consecutive run of free pages of length pageCount between start and end.
*   -Returns 0 on success (run found), 1 on failure (no run found). Writes start index of run to outIndex
* static void kernel_allocate(void)
*   -Reserves pages for the kernel code
* void bitmap_allocate(void)
*   -Reserves pages for the bitmap itself
*/
static BitmapLocation_t get_location(uint64_t pageIndex) {
	BitmapLocation_t location;
	location.rowIndex = pageIndex / BITS_PER_ROW;
	location.bitIndex = pageIndex % BITS_PER_ROW;
	return location;
}

static uint64_t page_to_address(uint64_t pageIndex){
	uint64_t address = (pageIndex * PAGE_SIZE);
	return address;
}

static uint64_t address_to_page(uint64_t address){
	uint64_t pageIndex = (address / PAGE_SIZE);
	return pageIndex;
}

static uint8_t bitmap_test(uint64_t pageIndex) {
	BitmapLocation_t location = get_location(pageIndex);
	uint32_t mask = (uint32_t) 1 << location.bitIndex;

	if ((pmm.bitmap[location.rowIndex] & mask) == 0 ){
		return 0;
	} else {
		return 1;
	}
}

static uint8_t bitmap_set(uint64_t pageIndex) {
	BitmapLocation_t location = get_location(pageIndex);
	uint32_t mask = (uint32_t) 1 << location.bitIndex;

	if (bitmap_test(pageIndex) == 0) {
		pmm.bitmap[location.rowIndex] |= mask;
		return 0;		
	} else {
		return 1;
	}
}

static uint8_t bitmap_clear(uint64_t pageIndex) {
	BitmapLocation_t location = get_location(pageIndex);
	uint32_t mask = (uint32_t) 1 << location.bitIndex;

	if (bitmap_test(pageIndex) == 1) {
		pmm.bitmap[location.rowIndex] &= ~mask;
		return 0;
	} else {
		return 1;
	}
}

static void bitmap_clear_region_callback(const MemoryRegion_t* region) {
	uint64_t startPage = region->base / PAGE_SIZE;
	uint64_t pageCount = region->len / PAGE_SIZE;
	for (uint64_t pageIndex = startPage; pageIndex < startPage + pageCount; pageIndex++) {
		bitmap_clear(pageIndex);
	}
}

static uint8_t bitmap_find_run(uint64_t start, uint64_t end, uint64_t pageCount, uint64_t* outIndex) {
	for (uint64_t pageIndex = start; pageIndex < end; pageIndex++) {
		if (bitmap_test(pageIndex)) {
			continue;
		}
		uint8_t found = 1;
		for (uint64_t offset = 0; offset < pageCount; offset++) {
            if (bitmap_test(pageIndex + offset)) {
                found = 0;
                break;
            }
        }
		if (found) {
			*outIndex = pageIndex;
			return 0;
		}
	}
	return 1;
}

/* -- Kernel/Bitmap Reservation -- */
static void kernel_allocate(void) {
	uint64_t totalKernelPages = 0;
	uint64_t kernelStartPage = (uint64_t)(uintptr_t)_physStart / PAGE_SIZE;
	uint64_t kernelEndPage = (uint64_t)((uintptr_t)_physEnd + PAGE_SIZE - 1) / PAGE_SIZE;
	for (uint64_t pageIndex = kernelStartPage; pageIndex < kernelEndPage; pageIndex ++) {
		bitmap_set(pageIndex);
		totalKernelPages++;
	}
}

static void bitmap_allocate(void) {
	uint64_t totalBitmapPages = 0;
	uint64_t bitmapStartPage = (uint64_t)(uintptr_t)pmm.bitmap / PAGE_SIZE;
	size_t bitmapMemSize = ((pmm.bitmapLength * sizeof(uint32_t)) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
	uint64_t bitmapEndPage = (bitmapStartPage + bitmapMemSize / PAGE_SIZE);
	for (uint64_t pageIndex = bitmapStartPage; pageIndex < bitmapEndPage; pageIndex ++) {
		bitmap_set(pageIndex);
		totalBitmapPages++;
	}
}

/* ---------------- Public API Implementation ---------------- 
* void pmm_init(uint64_t addr)
*	-Initialises the physical memory manager, placing bitmap at addr
* uint64_t palloc(uint64_t pageCount)
*	-Returns the physical address of an allocated page
*	-Returns UINT64_MAX on failure
* uint8_t pfree(uint64_t addr, uint64_t pageCount)
*	-Frees pageCount pages starting at addr
*	-Returns 0 if all pages were freed successfully. Returns 1 if any page failed
*/
void pmm_init(uint64_t addr) {
	uint64_t totalMem = (uint64_t)memorymap_get_max_addr();

	pmm.bitmap = (uint32_t*)((uintptr_t)((addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)));
	pmm.totalPages = totalMem / PAGE_SIZE;
	pmm.bitmapLength = ((pmm.totalPages + BITS_PER_ROW - 1) / BITS_PER_ROW);

	// for (size_t i = 0; i < pmm.bitmapLength; i++) {
	// 	pmm.bitmap[i] = 0xffffffff;
	// }

	memset(pmm.bitmap, 0xff, pmm.bitmapLength * BITS_PER_ROW);

	memorymap_foreach_usable(bitmap_clear_region_callback);
	kernel_allocate();
	bitmap_allocate();
}

uint64_t palloc(uint64_t pageCount) {
	uint8_t status = 0;
	uint64_t foundIndex = 0;

	if (pmm.bitmapLength == 0) {
		serial_writestring("NO MEMORY!! PANIC!!\n");
	} else {
		status = bitmap_find_run(pmm.lastAllocatedIndex, pmm.totalPages, pageCount, &foundIndex);
		if (status == 1) {
			status = bitmap_find_run(0, pmm.lastAllocatedIndex, pageCount, &foundIndex);
		}
	}

	if (status == 0) {
		uint64_t endIndex = foundIndex + pageCount;
		for (uint64_t pageIndex = foundIndex; pageIndex < endIndex; pageIndex++) {
			bitmap_set(pageIndex);
		}

		pmm.lastAllocatedIndex = foundIndex;

		return page_to_address(foundIndex);
	} else {
		return UINT64_MAX;
	}
}

uint8_t pfree(uint64_t addr, uint64_t pageCount) {
	uint64_t startIndex = address_to_page(addr);
	uint64_t endIndex = startIndex + pageCount;

	uint8_t status = 0;

	if (endIndex > pmm.totalPages) {
		endIndex = pmm.totalPages;
	}

	for (uint64_t pageIndex = startIndex; pageIndex < endIndex; pageIndex++) {
		if (bitmap_clear(pageIndex) == 1) {
			status = 1;
		} 
	}
	return status;
}
