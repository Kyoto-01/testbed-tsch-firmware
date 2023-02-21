#ifndef STDIO_H
#include <stdio.h>
#endif

void ascii_to_hex(char *input, char *output) {
    int i, j;

    i = j = 0;

    while (input[i] != '\0') {
        sprintf((char *)(output + j), "%02x", input[i]);
        i += 1;
        j += 2;
    }

    output[j] = '\0';
}