//
// Created by Luis Ruisinger on 30.09.23.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/filemanager.h"

uint8_t *
request_file(char *filename)
{
    if (!filename)
        return NULL;

    int chr;
    FILE* file;

    if ((file = fopen(filename, "r")) < 0)
        return NULL;

    fseek(file, 0, SEEK_END);
    uint64_t len = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *str = calloc(len + 1, sizeof(uint8_t));

#ifdef DEBUG
    assert(str);
#endif

    int32_t i = 0;
    while((chr = fgetc(file)) != EOF)
        str[i++] = (uint8_t ) chr & 0xFF;

    fclose(file);
    return str;
}