#include <stdint.h>
#include "arch/x86/memory/util/memUtil.h"

#define KERNEL_VOFFSET 0xC0000000

inline uintptr_t p2v(uint64_t physAddr) {
    return (uintptr_t)(physAddr + KERNEL_VOFFSET);
}

inline uint64_t v2p(uintptr_t vAddr) {
    return (uint64_t)(vAddr - KERNEL_VOFFSET);
}