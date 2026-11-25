#ifndef PMM_H_
#define PMM_H_

/* ---------------- Includes ---------------- */
#include <common/types.h>

/* ---------------- Public Constants ---------------- */
#define PALLOC_ANY       0
#define PALLOC_LOWMEM    1

#define PAGE_SIZE 4096

/* ---------------- Public Types ---------------- */
typedef uint8_t PallocFlags_t;

/* ---------------- Public API Prototypes ---------------- 
* void pmm_init(uint64_t addr)
*	-Initialises the physical memory manager, placing bitmap at addr
* uint64_t palloc(uint64_t pageCount, PallocFlags_t flags)
*	-Returns the physical address of an allocated page
*	-Returns UINT64_MAX on failure
* uint8_t pfree(uint64_t addr, uint64_t pageCount)
*	-Frees pageCount pages starting at addr
*	-Returns 0 if all pages were freed successfully. Returns 1 if any page failed
*/
void pmm_init(u64 addr);
u64 palloc(u64 pageCount, PallocFlags_t flags);
size_t pfree(u64 addr, u64 pageCount);

#endif