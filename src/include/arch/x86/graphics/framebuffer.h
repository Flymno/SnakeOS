#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

/* ---------------- Includes ---------------- */ 
#include <stdint.h>
#include <stddef.h>

/* ---------------- Public API Prototypes ---------------- 
* void framebuffer_multiboot2_init(const void* tag)
*	-Initialises framebuffer from a Multiboot2 framebuffer tag
*/
void framebuffer_multiboot2_init(const void* tag);

uint64_t framebuffer_get_address(void);
size_t framebuffer_get_pitch(void);
size_t framebuffer_get_width(void);
size_t framebuffer_get_height(void);
size_t framebuffer_get_bpp(void);
size_t framebuffer_get_type(void);
uint8_t framebuffer_get_red_field_pos(void);
uint8_t framebuffer_get_green_field_pos(void);
uint8_t framebuffer_get_blue_field_pos(void);

#endif