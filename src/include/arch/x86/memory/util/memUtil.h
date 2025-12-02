#ifndef MEMUTIL_H_
#define MEMUTIL_H_

#include <stdint.h>

uintptr_t p2v(uint64_t physAddr);
uint64_t v2p(uintptr_t vAddr);

#endif