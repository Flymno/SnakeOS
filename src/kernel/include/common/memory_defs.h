#ifndef SNAKEOS_MEMDEF_H_
#define SNAKEOS_MEMDEF_H_

#ifdef ARCH_X86_64
    #define KERNEL_VIRT_OFFSET 0xFFFFFFFF80000000
#elif ARCH_ARM
    #define KERNEL_VIRT_OFFSET 0x80000000 
#endif

#define v2p(addr) ((uintptr_t)(addr) - KERNEL_VIRT_OFFSET)
#define p2v(addr) ((uintptr_t)(addr) + KERNEL_VIRT_OFFSET)

#endif