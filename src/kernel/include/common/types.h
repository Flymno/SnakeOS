#ifndef SNAKEOS_TYPES_H_
#define SNAKEOS_TYPES_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uintptr_t virt_addr_t;
typedef uint64_t phys_addr_t;

typedef s32 status_t;
#define SUCCESS 0
#define FAILURE -1

typedef uintptr_t hal_arg_t;

#endif