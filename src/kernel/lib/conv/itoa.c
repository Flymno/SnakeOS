#include <common/types.h>

void itoa(u32 num, char* str, size_t base) {
    char* p = str;
    char* p1;
    char* p2;

    if (num == 0) {
        *p = '0';
        p++;
        *p = '\0';
        return;
    }

    char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    while (num != 0) {
        *p = digits[num % base];
        num /= base;
        p++;
    }
    *p = '\0';

    p1 = str;
    p2 = p - 1;

    while (p1 < p2) {
        char tmp = *p2;
        *p2 = *p1;
        *p1 = tmp;
        p1++;
        p2--;
    }
}