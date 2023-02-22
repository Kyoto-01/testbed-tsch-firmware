#ifndef STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#endif

static uint8_t *ascii_to_hex(uint8_t *input, uint32_t inputlen) {
    uint8_t *tmp = (uint8_t *)malloc(sizeof(uint8_t) * inputlen * 2);
    uint32_t i, j;

    i = j = 0;

    while (i < inputlen) {
        sprintf((char *)(tmp + j), "%02x", input[i]);
        i += 1;
        j += 2;
    }

    tmp[j] = '\0';

    return tmp;
}