#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

/* ---------------- Includes ---------------- */ 
#include <stdint.h>

/* ---------------- Public Types ---------------- */ 
typedef struct {
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t  bpp;
    uint8_t  type;

    uint8_t redPos;
    uint8_t greenPos;
    uint8_t bluePos;

} Framebuffer_t;

/* ---------------- Public State ---------------- */ 
extern Framebuffer_t framebuffer;

/* ---------------- Public API Prototypes ---------------- 
* void framebuffer_multiboot2_init(const void* tag)
*	-Initialises framebuffer from a Multiboot2 framebuffer tag
*/
void framebuffer_multiboot2_init(const void* tag);

#endif