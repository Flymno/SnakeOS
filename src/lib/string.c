#include <stddef.h>
#include <stdint.h>
#include "lib/string.h"
#include "drivers/serial/serial.h"

void * memcpy(void *dest, const void *src, size_t n) {
	const unsigned char *csrc = (const unsigned char *)src;
	unsigned char *cdest = (unsigned char *)dest;

	for (size_t i = 0; i < n; i++) {
		cdest[i] = csrc[i];
	}

	return dest;
}

void * memset(void *dest, uint8_t value, size_t n) {
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
	size_t src_len = strlen(src);
	size_t n = dest_len + src_len;
	size_t i;

	for (i = 0; i < n && src[i] != '\0'; i++) {
		dest[dest_len + i] = src[i];
	};
	dest[dest_len + i] = '\0';
	return dest;
}
