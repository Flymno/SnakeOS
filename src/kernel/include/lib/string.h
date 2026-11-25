#ifndef STRING_H_
#define STRING_H_

#include <common/types.h>

void * memcpy(void *dest, const void *src, size_t n);

void * memset(void *dest, u8 value, size_t n);

char* strcpy(char* dest, const char* src);

size_t strlen(const char* str);

char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);

char* strchr(const char* src, u8 chr);

s8 strcmp(const char* str1, const char* str2);

#endif