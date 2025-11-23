#ifndef MULTIBOOT2_H_
#define MULTIBOOT2_H_

#include <stdint.h>

int verify_multiboot(uint32_t multiboot2_magic);

typedef void (*multiboot2_tag_callback_t)(const void *tag);
void multiboot2_register_callback(multiboot2_tag_callback_t callback);
void multiboot2_parse(uint32_t multiboot2_info_addr);

#endif