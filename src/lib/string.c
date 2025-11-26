#include <stddef.h>
#include <stdint.h>
#include "lib/string.h"
#include "drivers/serial/serial.h"

void* memcpy(void *dest, const void *src, size_t n) {
	const unsigned char *csrc = (const unsigned char *)src;
	unsigned char *cdest = (unsigned char *)dest;

	for (size_t i = 0; i < n; i++) {
		cdest[i] = csrc[i];
	}

	return dest;
}

void* memset(void *dest, uint8_t value, size_t n) {
	unsigned char *cdest = (unsigned char *)dest;
	unsigned char cvalue = (unsigned char)value;

	for (size_t i = 0; i < n; i++) {
		cdest[i] = cvalue;
	}

	return dest;
}

char* strcpy(char* dest, const char* src) {
	size_t len = 0;
	len = strlen(src);
	for (size_t i = 0; i < len; i++) {
		dest[i] = src[i];
	}
	return dest;
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

const char* strcat(char* dest, const char* src) {
	size_t dest_len = strlen(dest);
	for (size_t i = dest_len; src[i - dest_len] != '\0'; i++) {
		dest[i] = src[i - dest_len];
	};
	return dest;
}

const char* strncat(char* dest, const char* src, size_t n) {
	size_t dest_len = strlen(dest);
	for (size_t i = dest_len; i < (n + dest_len) && src[i - dest_len] != '\0'; i++) {
		dest[i] = src[i - dest_len];
	};
	return dest;
}

char* strchr(const char* src, uint8_t chr) {
	for (size_t i = 0; src[i] != '\0'; i++) {
		if (src[i] == (char)chr) {
			return (char*)src + i;
		}
	}
	return NULL;
}

int8_t strcmp(const char* str1, const char* str2) {
	size_t i = 0;
	for (i = 0; str1[i] != '\0' || str2[i] != '\0'; i++) {
		if (str1[i] != str2[i]) {
			return (int8_t)(str1[i] - str2[i]);
		}
	}
	return 0;
}