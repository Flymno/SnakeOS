#ifndef MULTIBOOT2_CONSTS_H_
#define MULTIBOOT2_CONSTS_H_

/* ---------------- Includes ---------------- */
#include <stdint.h>

/* ---------------- Public Constants ---------------- */
#define MULTIBOOT2_MAGIC 0x36d76289

/* ---------------- Public Types ---------------- */

/* Multiboot tag types (from spec) */
typedef enum {
	TAG_END = 0,
	TAG_MMAP = 6,
	TAG_FRAMEBUFFER = 8
} Multiboot2TagType;

/* framebuffer types */
typedef enum {
	FRAMEBUFFER_INDEXED = 0,
	FRAMEBUFFER_RGB,
	FRAMEBUFFER_EGA_TEXT
} Multiboot2FramebufferType;

/* MMemory map entry types */
typedef enum {
	MEMORY_AVAILABLE = 1,
	MEMORY_RESERVED,
	MEMORY_ACPI_RECLAIMABLE,
	MEMORY_NVS,
	MEMORY_BADRAM
} Multiboot2MemoryRegionType;

#endif
