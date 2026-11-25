#ifndef SLAB_H_
#define SLAB_H_

#include <common/types.h>

#define SLAB_MAGIC 0x534C4142

//forward declarations are necessary here
typedef struct Slot_s Slot_t;
typedef struct SlabPage_s SlabPage_t;

typedef struct Slot_s {
    Slot_t* next;
} Slot_t;

typedef struct SlabPage_s {
    u32 checksum;
    SlabPage_t* next;
    Slot_t* freeList;
    size_t freeCount;
    size_t objSize;
} SlabPage_t;


void* slab_alloc(void** bucketHead, size_t objSize);
bool slab_free(void* obj);

#endif