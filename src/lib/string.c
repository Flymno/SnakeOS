#include <stddef.h>
#include "lib/string.h"

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
