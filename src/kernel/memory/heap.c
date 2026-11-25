#include <common/types.h>
#include <memory/slab.h>
#include <lib/string.h>
#include <memory/pmm.h>
#include <common/memory_defs.h>
#include <lib/stdio.h>

SlabPage_t* buckets[9];
size_t sizes[9] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048};

void kfree(void* obj);

typedef struct BigAllocation_s BigAllocation_t;
typedef struct BigAllocation_s {
    uintptr_t baseAddr;
    size_t pageCount;
    BigAllocation_t* next;
} BigAllocation_t;

BigAllocation_t* bigAllocationList;

static s32 get_bucket_index(size_t objSize) {
    for (size_t i = 0; i < 9; i++) {
        if (objSize <= sizes[i]) return i;
    }
    return -1;
};

void kfree(void* obj);

static bool big_alloc_free(void* obj) {
    BigAllocation_t* curr = bigAllocationList;
    BigAllocation_t* prev = NULL;

    while (curr != NULL && curr->baseAddr != (uintptr_t)obj)
    {
        prev = curr;
        curr = curr->next;
    }
    if (curr == NULL) return false;
    
    pfree(v2p(curr->baseAddr), curr->pageCount);

    if (prev == NULL) {
        bigAllocationList = curr->next;
    } else {
        prev->next = curr->next;
    }

    kfree(curr);

    return true;
}

void* kmalloc(size_t objSize) {
    s32 index = get_bucket_index(objSize);

    if (index != -1) {
        return slab_alloc((void**)&buckets[index], sizes[index]);
    }

    size_t pageCount = (objSize + PAGE_SIZE - 1) / PAGE_SIZE;

    void* allocatedPages = (void*)palloc(pageCount, PALLOC_LOWMEM);
    if (allocatedPages == NULL) {
        return NULL;
    }

    BigAllocation_t* newAlloc = kmalloc(sizeof(BigAllocation_t));
    if (!newAlloc) {
        pfree(v2p((uintptr_t)allocatedPages), pageCount);
        return NULL;
    }

    newAlloc->baseAddr = p2v((uintptr_t)allocatedPages);
    newAlloc->pageCount = pageCount;

    newAlloc->next = bigAllocationList;
    bigAllocationList = newAlloc;

    return (void*)newAlloc->baseAddr;
}

void* kcalloc(size_t nmemb, size_t objSize) {
    if (nmemb != 0 && objSize > (size_t)-1 / nmemb) return NULL;
    
    size_t totalSize = nmemb * objSize;

    void* ptr = kmalloc(totalSize);
    if(ptr) {
        memset(ptr, 0x0, totalSize);
        return ptr;
    }
    return NULL;
}

void kfree(void* obj) {
    if (obj == NULL) return;

    if (slab_free(obj)) return ;

    if (big_alloc_free(obj)) return;

    //something has gone wrong
    printf("\n> WARN: Tried to free invalid pointer! Pointer: %#08x\n", obj);
    return;
}