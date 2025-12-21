#ifndef STRING_H_
#define STRING_H_

#include <stddef.h>

void * memcpy(void *dest, const void *src, size_t n);

void * memset(void *dest, uint8_t value, size_t n);

char* strcpy(char* dest, const char* src);

size_t strlen(const char* str);

const char* strcat(char* dest, const char* src);
const char* strncat(char* dest, const char* src, size_t n);

char* strchr(const char* src, uint8_t chr);

int8_t strcmp(const char* str1, const char* str2);

#endif