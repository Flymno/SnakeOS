/* ---------------- Includes ---------------- */
#include <stdint.h>

/* ---------------- Internal Constants ---------------- */
#define PDE_PRESENT 0x1
#define PDE_RW 0x2
#define PDE_PS 0x80

/* ---------------- Internal State ---------------- */
__attribute__((section(".bootstrap.bss"), aligned(4096)))
uint32_t earlyPageDirectory[1024];

/* ---------------- Public API Implementation ---------------- 
* uint32_t* init_early_paging()
*   -Sets up the initial corase page directories to jump to a high-half kernel
*/
__attribute__((section(".bootstrap.text")))
uint32_t* init_early_paging() {
    // Identity map 8M
    earlyPageDirectory[0] = 0x00000000 | PDE_PRESENT | PDE_RW | PDE_PS;
    earlyPageDirectory[1] = 0x00400000 | PDE_PRESENT | PDE_RW | PDE_PS;

    // Map 8M to high-half
    earlyPageDirectory[768] = 0x00000000 | PDE_PRESENT | PDE_RW | PDE_PS;
    earlyPageDirectory[769] = 0x00400000 | PDE_PRESENT | PDE_RW | PDE_PS;

    return earlyPageDirectory;
}