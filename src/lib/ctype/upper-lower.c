#include <common/types.h>
#include <lib/ctype.h>

u8 isupper(u8 c) {
    return (c >= 'A') && (c <= 'Z');
}

u8 islower(u8 c) {
    return (c >= 'a') && (c <= 'z');
}

u8 toupper(u8 c) {
    if (islower(c)) {
        return c - 32;
    }
    return c;
}

u8 tolower(u8 c) {
    if (isupper(c)) {
        return c + 32;
    }
    return c;
}