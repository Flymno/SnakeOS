#ifndef HEAP_H_
#define HEAP_H_

#include <common/types.h>

void* kmalloc(size_t objSize);
void* kcalloc(size_t nmemb, size_t objSize);
void kfree(void* obj);

#endif