#ifndef MULTIBOOT2_H_
#define MULTIBOOT2_H_

/* ---------------- Includes ---------------- */
#include <stdint.h>

/* ---------------- Public Types ---------------- */
typedef void (*multiboot2_tag_callback_t)(const void *tag);

/* ---------------- Public API Prototypes ----------------
* uint8_t multiboot2_verify(uint32_t multiboot2Magic)
*	-Returns 0 if Multiboot2 was successful. Returns 1 otherwise
* void multiboot2_register_callback(multiboot2_tag_callback_t cb)
*	-Registers a callback for Multiboot2 initialisation
* void multiboot2_init(uint32_t multiboot2InfoAddr)
*	-Parses Multiboot2 info and calls registered callbacks
*/
uint8_t multiboot2_verify(uint32_t multiboot2Magic);
void multiboot2_register_callback(multiboot2_tag_callback_t callback);
void multiboot2_init(uint32_t multiboot2InfoAddr);

#endif