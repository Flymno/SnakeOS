#include <common/types.h>
#include <common/memory_defs.h>
#include <memory/pmm.h>
#include <memory/slab.h>
#include <lib/stdio.h>

static u32 slab_get_checksum(SlabPage_t* slab) {
    return SLAB_MAGIC ^ ((uintptr_t)slab) ^ slab->objSize;
}

static void slab_init_new(SlabPage_t** slabList, size_t objSize) {
    //if (heapInit == false)
    void* rawMem = (void*)palloc(1, PALLOC_LOWMEM); //Before full VMM initialisation, we can only access lower 8MB of RAM
    //else {void* rawMem = kmalloc(1)}
    if (rawMem) {
        rawMem = (void*)p2v((uintptr_t)rawMem);
    } else {
        printf("\nPANIC! FATAL: Out of memory\n");
    }

    if (objSize < sizeof(Slot_t)) objSize = sizeof(Slot_t);
    objSize = (objSize + 7) & ~7;

    SlabPage_t* slab = (SlabPage_t*)rawMem;

    slab->next = NULL;
    slab->freeList = NULL;
    size_t maxObjects = (PAGE_SIZE - sizeof(SlabPage_t)) / objSize;
    slab-> freeCount = maxObjects;
    slab->objSize = objSize;

    slab->checksum = slab_get_checksum(slab);

    uintptr_t currentObj = (uintptr_t)slab + sizeof(SlabPage_t);
    
    for (size_t i = 0; i < maxObjects; i++) {
        Slot_t* currentSlot = (Slot_t*)currentObj;

        currentSlot->next = slab->freeList;
        slab->freeList = currentSlot;

        currentObj += objSize;
    }

    if (*slabList != NULL) {
        slab->next = *slabList;
    }
    *slabList = slab;
}

void* slab_alloc(void** bucketHead, size_t objSize) {
    SlabPage_t* search = (SlabPage_t*)*bucketHead;

    while (search != NULL && search->freeCount == 0) {
        search = search->next;
    }

    if (search == NULL) {
        slab_init_new((SlabPage_t**)bucketHead, objSize);
        search = (SlabPage_t*)*bucketHead;
    }

    Slot_t* currentSlot = search->freeList;
    search->freeList = currentSlot->next;
    search->freeCount--;

    return (void*)currentSlot;
}

bool slab_free(void* obj) {
    SlabPage_t* currentSlab = (SlabPage_t*)((uintptr_t)obj & ~(PAGE_SIZE - 1));

    u32 checksum = slab_get_checksum(currentSlab);
    if (checksum != currentSlab->checksum) return false;

    Slot_t* currentSlot = (Slot_t*)obj;
    currentSlot->next = currentSlab->freeList;
    currentSlab->freeList = currentSlot;
    currentSlab->freeCount++;

    return true;
}