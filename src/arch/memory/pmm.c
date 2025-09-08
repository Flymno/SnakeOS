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

extern char _scode[], _end[];

int lastAllocatedIndex = 0;

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
	struct bitmapLocation location = get_location(pageIndex);

	pageIndex = (location.rowIndex * BITS_PER_ROW) + location.bitIndex;
	uintptr_t address = (pageIndex * PAGE_SIZE) + (uintptr_t)bitmap;
	return address;
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
		return 0;		
	} else {
		return 1;
	}
}


uint8_t free_page(uintptr_t pageIndex) {
	struct bitmapLocation location = get_location(pageIndex);

	uint32_t mask = (uint32_t) 1 << location.bitIndex;

	if (is_page_allocated(pageIndex) == 1) {
		bitmap[location.rowIndex] &= ~mask;
		return 0;
	} else {
		return 1;
	}
}


void init_bitmap_allocator(uintptr_t bitmap_addr) {
	uint64_t totalMem = memoryMap[memoryMap_Size - 1].addr + memoryMap[memoryMap_Size - 1].len;

	bitmap = (uint32_t*)((bitmap_addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
	bitmapLength = ((totalMem / PAGE_SIZE) + BITS_PER_ROW - 1) / BITS_PER_ROW;

	for (size_t i = 0; i < bitmapLength; i++) {
		bitmap[i] = 0xffffffff;
	}


	uint32_t totalUsablePages = 0;
	for (size_t i = 0; i < usableMemoryMap_Size; i++) {
		uintptr_t startPage = usableMemoryMap[i].base / PAGE_SIZE;
		size_t pageCount = usableMemoryMap[i].len / PAGE_SIZE;
		uintptr_t endPage = startPage + pageCount;
		for (uintptr_t pageIndex = startPage; pageIndex < endPage; pageIndex ++) {
			free_page(pageIndex);
			totalUsablePages++;
		}
	}
	serial_writestring("Freed ");
	serial_writedec(totalUsablePages);
	serial_writestring(" usable pages\n");

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
	size_t bitmapMemSize = bitmapLength * sizeof(uint32_t);
	uintptr_t bitmapEndAddr = (uintptr_t)bitmap + bitmapMemSize;
	uintptr_t bitmapEndPage = ((uintptr_t)bitmapEndAddr + PAGE_SIZE - 1) / PAGE_SIZE;
	for (uintptr_t pageIndex = bitmapStartPage; pageIndex < bitmapEndPage; pageIndex ++) {
		allocate_page(pageIndex);
		totalBitmapPages++;
	}
	serial_writestring("Reserved ");
	serial_writedec(totalBitmapPages);
	serial_writestring(" bitmap pages\n");

}

uintptr_t palloc(uint32_t pageCount) {
	uint8_t found = 0;
	uintptr_t foundIndex = 0;
	uint32_t currentPagecount = 0;

	serial_writestring("definitionas successful");

	for (uintptr_t pageIndex = 0; (pageIndex < bitmapLength) && (found = 0) ; pageIndex = (pageIndex + 1) % bitmapLength) {
		serial_writestring("loop");
		if (is_page_allocated(pageIndex) == 0) {
			found = 1;
			serial_writestring("found a free page");
			for (uintptr_t nextIndex = pageIndex + 1; currentPagecount < pageCount; nextIndex = (nextIndex + 1) % bitmapLength){
				currentPagecount ++;
				if (is_page_allocated(nextIndex) == 1) {
					found = 0;
				}
			}
		}
		if (found == 1) {
			foundIndex = pageIndex;
		}
	}

	for (uintptr_t pageIndex = foundIndex; pageIndex < pageIndex + pageCount; pageIndex = (pageIndex + 1) % bitmapLength) {
		allocate_page(pageIndex);
	}

	return get_address(foundIndex);
}