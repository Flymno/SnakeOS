/* ---------------- Includes ---------------- */
#include <common/types.h>
#include <common/memory_defs.h>
#include <memory/map.h>
#include <memory/pmm.h>
#include <lib/string.h>
#include <lib/stdio.h>

/* ---------------- Internal Constants ---------------- */
#define BITS_PER_ROW 32

/* ---------------- Extern Symbols ---------------- */
extern char _physStart[], _physEnd[];

/* ---------------- Internal Types ---------------- */
typedef struct
{
	u32* bitmap;
	size_t bitmapLength;
	u64 totalPages;
	u64 lastAllocatedIndex;
} PMMState_t;

typedef struct 
{
	size_t rowIndex;
	size_t bitIndex;
} BitmapLocation_t;

/* ---------------- Internal State ---------------- */
static PMMState_t pmm;

/* ---------------- Internal Helper Prototypes ---------------- */
static BitmapLocation_t get_location(u64 pageIndex);
static u64 page_to_address(u64 pageIndex);
static u64 address_to_page(u64 address);
static size_t bitmap_test(u64 pageIndex);
static size_t bitmap_set(u64 pageIndex);
static size_t bitmap_clear(u64 pageIndex);
static void bitmap_clear_region_callback(const MemoryRegion_t* region);
static size_t bitmap_find_run(u64 start, u64 end, u64 pageCount, u64* outIndex);
static void low_reserve(void);
static void bitmap_allocate(void);
static void high_reserve(void);

/* ---------------- Internal Helper Implementation ----------------
* static BitmapLocation_t get_location(uint64_t pageIndex)
*   -Returns the location of a page's bit in the bitmap (row and column)
* static uint64_t page_to_address(uint64_t pageIndex)
*   -Converts a page index to a physical address
* static uint64_t address_to_page(uint64_t address)
*   -Converts a physical address to a page index
* static size_t bitmap_test(uint64_t pageIndex)
*   -Checks if a page is allocated. Returns 0 if free, 1 if allocated
* static size_t bitmap_set(uint64_t pageIndex)
*   -Marks a page as allocated. Returns 0 on success, 1 on failure
* static size_t bitmap_clear(uint64_t pageIndex)
*   -Marks a page as free. Returns 0 on success, 1 on failure
* static void bitmap_clear_region_callback(const MemoryRegion_t* region)
*   -Clears all pages in the bitmap corresponding to the given memory region
* static size_t bitmap_find_run(uint64_t start, uint64_t end, uint64_t pageCount, uint64_t* outIndex)
*   -Finds a consecutive run of free pages of length pageCount between start and end.
*   -Returns 0 on success (run found), 1 on failure (no run found). Writes start index of run to outIndex
* static void low_reserve(void)
*   -Reserves pages for the kernel code, early paging structures, and low-mem
* static void bitmap_allocate(void)
*   -Reserves pages for the bitmap itself
* static void high_reserve(void)
*	-Reserves all pages above 4GB as they are not accessible in protected mode
*/
static BitmapLocation_t get_location(u64 pageIndex) {
	BitmapLocation_t location;
	location.rowIndex = pageIndex / BITS_PER_ROW;
	location.bitIndex = pageIndex % BITS_PER_ROW;
	return location;
}

static u64 page_to_address(u64 pageIndex){
	u64 address = (pageIndex * PAGE_SIZE);
	return address;
}

static u64 address_to_page(u64 address){
	u64 pageIndex = (address / PAGE_SIZE);
	return pageIndex;
}

static size_t bitmap_test(u64 pageIndex) {
	BitmapLocation_t location = get_location(pageIndex);
	u32 mask = (u32) 1 << location.bitIndex;

	if ((pmm.bitmap[location.rowIndex] & mask) == 0 ){
		return 0;
	} else {
		return 1;
	}
}

static size_t bitmap_set(u64 pageIndex) {
	BitmapLocation_t location = get_location(pageIndex);
	u32 mask = (u32) 1 << location.bitIndex;

	if (bitmap_test(pageIndex) == 0) {
		pmm.bitmap[location.rowIndex] |= mask;
		return 0;		
	} else {
		return 1;
	}
}

static size_t bitmap_clear(u64 pageIndex) {
	BitmapLocation_t location = get_location(pageIndex);
	u32 mask = (u32) 1 << location.bitIndex;

	if (bitmap_test(pageIndex) == 1) {
		pmm.bitmap[location.rowIndex] &= ~mask;
		return 0;
	} else {
		return 1;
	}
}

static void bitmap_clear_region_callback(const MemoryRegion_t* region) {
	u64 startPage = region->baseAddr / PAGE_SIZE;
	u64 pageCount = region->regionLen / PAGE_SIZE;
	for (u64 pageIndex = startPage; pageIndex < startPage + pageCount; pageIndex++) {
		bitmap_clear(pageIndex);
	}
}

static size_t bitmap_find_run(u64 start, u64 end, u64 pageCount, u64* outIndex) {
	for (u64 pageIndex = start; pageIndex < end;) {
		if (pageIndex + pageCount > end) {
			return 1;
		}

		if ((pageIndex % 32 == 0) && (pmm.bitmap[pageIndex / 32] == 0xFFFFFFFF)) {
            pageIndex += 32;
            continue;
        }

		if (bitmap_test(pageIndex)) {
			pageIndex ++;
			continue;
		}

		size_t found = 1;
		for (u64 offset = 0; offset < pageCount; offset++) {
			u64 current = pageIndex + offset;

            if (current >= end || bitmap_test(current)) {
                found = 0;
				pageIndex = current + 1;
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
static void low_reserve(void) {
	u64 kernelEndPage = (u64)((uintptr_t)_physEnd + PAGE_SIZE - 1) / PAGE_SIZE;
	for (u64 pageIndex = 0; pageIndex < kernelEndPage; pageIndex ++) {
		bitmap_set(pageIndex);
	}
}

static void bitmap_allocate(void) {
	u64 bitmapStartPage = (u64)v2p((uintptr_t)pmm.bitmap) / PAGE_SIZE;
	size_t bitmapMemSize = ((pmm.bitmapLength * sizeof(u32)) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
	u64 bitmapEndPage = (bitmapStartPage + bitmapMemSize / PAGE_SIZE);
	for (u64 pageIndex = bitmapStartPage; pageIndex < bitmapEndPage; pageIndex ++) {
		bitmap_set(pageIndex);
	}
}

static void high_reserve(void) {
	u64 limitBoundaryPage = ((u64)0x100000000ULL) / PAGE_SIZE;
	if (pmm.totalPages > limitBoundaryPage) {
		for (u64 pageIndex = limitBoundaryPage; pageIndex < pmm.totalPages; pageIndex ++) {
			bitmap_set(pageIndex);
		}
	}
}

/* ---------------- Public API Implementation ---------------- 
* void pmm_init(uint64_t addr)
*	-Initialises the physical memory manager, placing bitmap at addr
* uint64_t palloc(uint64_t pageCount, PallocFlags_t flags)
*	-Returns the physical address of an allocated page
*	-Returns UINT64_MAX on failure
* uint8_t pfree(uint64_t addr, uint64_t pageCount)
*	-Frees pageCount pages starting at addr
*	-Returns 0 if all pages were freed successfully. Returns 1 if any page failed
*/
void pmm_init(u64 addr) {
	u64 totalMem = (u64)memory_map_get_max_addr_usable();

	pmm.bitmap = (u32*)((uintptr_t)((addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)));
	pmm.totalPages = totalMem / PAGE_SIZE;
	pmm.bitmapLength = ((pmm.totalPages + BITS_PER_ROW - 1) / BITS_PER_ROW);

	// for (size_t i = 0; i < pmm.bitmapLength; i++) {
	// 	pmm.bitmap[i] = 0xffffffff;
	// }

	memset(pmm.bitmap, 0xff, pmm.bitmapLength * sizeof(uint32_t));

	memory_map_foreach_usable(bitmap_clear_region_callback);
	low_reserve();
	bitmap_allocate();
	high_reserve();
	bitmap_set(address_to_page(0x0));

	printf("> Bitmap Initialised! Size: %#08x\n", ((pmm.bitmapLength * sizeof(u32)) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));

	pmm.lastAllocatedIndex = address_to_page(0x00800000);
}

u64 palloc(u64 pageCount, PallocFlags_t flags) {
	size_t status = 0;
	u64 foundIndex = 0;

	if (pmm.bitmapLength == 0) {
		printf("NO MEMORY!! PANIC!!\n");
	} else if (flags == PALLOC_ANY) {
		status = bitmap_find_run(pmm.lastAllocatedIndex, pmm.totalPages, pageCount, &foundIndex);
		if (status == 1) {
			status = bitmap_find_run(address_to_page(0x00800000), pmm.lastAllocatedIndex, pageCount, &foundIndex);
		}
	} else if (flags == PALLOC_LOWMEM) {
		status = bitmap_find_run(0, address_to_page(0x00800000), pageCount, &foundIndex);
	}

	if (status == 0) {
		u64 endIndex = foundIndex + pageCount;
		for (u64 pageIndex = foundIndex; pageIndex < endIndex; pageIndex++) {
			bitmap_set(pageIndex);
		}

		pmm.lastAllocatedIndex = foundIndex + pageCount;

		return page_to_address(foundIndex);
	} else {
		return 0;
	}
}

size_t pfree(u64 addr, u64 pageCount) {
	u64 startIndex = address_to_page(addr);
	u64 endIndex = startIndex + pageCount;

	size_t status = 0;

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
