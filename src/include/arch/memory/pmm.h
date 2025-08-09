#ifndef PMM_H_
#define PMM_H_

#include <stddef.h>
#include "arch/boot/multiboot2/multiboot2.h"
#include "arch/memory/memoryMap.h"


void init_bitmap_allocator(uintptr_t* bitmap_addr);

#endif