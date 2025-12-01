#ifndef PMM_H_
#define PMM_H_

/* ---------------- Includes ---------------- */
#include <stddef.h>
#include "arch/boot/multiboot2/multiboot2.h"
#include "arch/memory/memoryMap.h"

/* ---------------- Public API Prototypes ---------------- 
* void pmm_init(uint64_t addr)
*	-Initialises the physical memory manager, placing bitmap at addr
* uint64_t palloc(uint64_t pageCount)
*	-Returns the physical address of an allocated page
*	-Returns UINT64_MAX on failure
* uint8_t pfree(uint64_t addr, uint64_t pageCount)
*	-Frees pageCount pages starting at addr
*	-Returns 0 if all pages were freed successfully. Returns 1 if any page failed
*/
void pmm_init(uint64_t addr);
uint64_t palloc(uint64_t pageCount);
uint8_t pfree(uint64_t addr, uint64_t pageCount);

#endif