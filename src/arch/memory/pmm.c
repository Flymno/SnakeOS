#include <stdint.h>
#include <stddef.h>
#include "arch/memory/pmm.h"
#include "arch/boot/multiboot2/multiboot2.h"
#include "arch/memory/memoryMap.h"
#include "drivers/serial/serial.h"

#define PAGE_SIZE 4096
#define BITS_PER_ROW 32

static uint32_t* bitmap;
static size_t bitmapLength;
static uintptr_t totalPages;

extern char _scode[], _end[];

static uintptr_t lastAllocatedIndex = 0;

struct bitmapLocation
{
	uint32_t rowIndex;
	uint8_t bitIndex;
};

struct bitmapLocation get_location(uintptr_t pageIndex) {
	struct bitmapLocation location;
	location.rowIndex = pageIndex / BITS_PER_ROW;
	location.bitIndex = pageIndex % BITS_PER_ROW;
	return location;
}

uintptr_t get_address(uintptr_t pageIndex){
	uintptr_t address = (pageIndex * PAGE_SIZE);
	return address;
}

uintptr_t get_index(uintptr_t address){
	uintptr_t pageIndex = (address / PAGE_SIZE);
	return pageIndex;
}



uint8_t is_page_allocated(uintptr_t pageIndex) {
	struct bitmapLocation location = get_location(pageIndex);
	uint32_t mask = (uint32_t) 1 << location.bitIndex;

	if ((bitmap[location.rowIndex] & mask) == 0 ){
		return 0;
	} else {
		return 1;
	}
}

uint8_t allocate_page(uintptr_t pageIndex) {
	struct bitmapLocation location = get_location(pageIndex);

	uint32_t mask = (uint32_t) 1 << location.bitIndex;

	if (is_page_allocated(pageIndex) == 0) {
		bitmap[location.rowIndex] |= mask;
		return 1;		
	} else {
		return 0;
	}
}


uint8_t free_page(uintptr_t pageIndex) {
	struct bitmapLocation location = get_location(pageIndex);

	uint32_t mask = (uint32_t) 1 << location.bitIndex;

	if (is_page_allocated(pageIndex) == 1) {
		bitmap[location.rowIndex] &= ~mask;
		return 1;
	} else {
		return 0;
	}
}

static void free_region_callback(memoryRegion_t* region) {
	uintptr_t startPage = region->base / PAGE_SIZE;
	uintptr_t pageCount = region->len / PAGE_SIZE;
	for (uintptr_t pageIndex = startPage; pageIndex < startPage + pageCount; pageIndex++) {
		free_page(pageIndex);
	}
}

void init_bitmap_allocator(uintptr_t bitmap_addr) {
	uint64_t totalMem = (uint64_t)memorymap_get_highest_address();

	bitmap = (uint32_t*)((bitmap_addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
	bitmapLength = ((totalMem / PAGE_SIZE) + BITS_PER_ROW - 1) / BITS_PER_ROW;

	totalPages = totalMem / PAGE_SIZE;

	for (size_t i = 0; i < bitmapLength; i++) {
		bitmap[i] = 0xffffffff;
	}


	memorymap_foreach_usable(free_region_callback);

	// uint32_t totalUsablePages = 0;
	// for (size_t i = 0; i < usableMemoryMap_Size; i++) {
	// 	uintptr_t startPage = usableMemoryMap[i].base / PAGE_SIZE;
	// 	size_t pageCount = usableMemoryMap[i].len / PAGE_SIZE;
	// 	uintptr_t endPage = startPage + pageCount;
	// 	for (uintptr_t pageIndex = startPage; pageIndex < endPage; pageIndex ++) {
	// 		free_page(pageIndex);
	// 		totalUsablePages++;
	// 	}
	// }
	// serial_writestring("Freed ");
	// serial_writedec(totalUsablePages);
	// serial_writestring(" usable pages\n");

	uint32_t totalKernelPages = 0;
	uintptr_t kernelStartPage = (uintptr_t)_scode / PAGE_SIZE;
	uintptr_t kernelEndPage = ((uintptr_t)_end + PAGE_SIZE - 1) / PAGE_SIZE;
	for (uintptr_t pageIndex = kernelStartPage; pageIndex < kernelEndPage; pageIndex ++) {
		allocate_page(pageIndex);
		totalKernelPages++;
	}
	serial_writestring("Reserved ");
	serial_writedec(totalKernelPages);
	serial_writestring(" kernel pages\n");

	uint32_t totalBitmapPages = 0;
	uintptr_t bitmapStartPage = (uintptr_t)bitmap / PAGE_SIZE;
	size_t bitmapMemSize = ((bitmapLength * sizeof(uint32_t)) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
	uintptr_t bitmapEndPage = (bitmapStartPage + bitmapMemSize / PAGE_SIZE);
	for (uintptr_t pageIndex = bitmapStartPage; pageIndex < bitmapEndPage; pageIndex ++) {
		allocate_page(pageIndex);
		totalBitmapPages++;
	}
	serial_writestring("Reserved ");
	serial_writedec(totalBitmapPages);
	serial_writestring(" bitmap pages\n");

}

int find_consecutive_free_pages(uintptr_t start, uintptr_t end, uint32_t pageCount, uintptr_t* outIndex) {
	for (uintptr_t pageIndex = start; pageIndex < end; pageIndex++) {
		if (is_page_allocated(pageIndex)) {
			continue;
		}
		uint8_t found = 1;
		for (uintptr_t offset = 0; offset < pageCount; offset++) {
            if (is_page_allocated(pageIndex + offset)) {
                found = 0;
                break;
            }
        }
		if (found) {
			*outIndex = pageIndex;
			return 1;
		}
	}
	return 0;
}

uintptr_t palloc(uint32_t pageCount) {
	uint8_t success = 0;
	uintptr_t foundIndex = 0;

	if (bitmapLength == 0) {
		serial_writestring("NO MEMORY!! PANIC!!\n");
	} else {
		/* Pass one */
		success = find_consecutive_free_pages(lastAllocatedIndex, totalPages, pageCount, &foundIndex);
		/* Pass two */
		if (!success) {
			success = find_consecutive_free_pages(0, lastAllocatedIndex, pageCount, &foundIndex);
		}
	}

	if (success) {
		uintptr_t endIndex = foundIndex + pageCount;
		for (uintptr_t pageIndex = foundIndex; pageIndex < endIndex; pageIndex++) {
			allocate_page(pageIndex);
		}

		lastAllocatedIndex = foundIndex;

		return get_address(foundIndex);
	} else {
		return UINTPTR_MAX;
	}
}

uint8_t pfree(uintptr_t address, uint32_t pageCount) {
	uintptr_t startIndex = get_index(address);
	uintptr_t endIndex = startIndex + pageCount;

	uint8_t success = 1;

	if (endIndex > totalPages) {
		endIndex = totalPages;
	}

	for (uintptr_t pageIndex = startIndex; pageIndex < endIndex; pageIndex++) {
		if (!free_page(pageIndex)) {
			success = 0;
		} 
	}
	return success;
}