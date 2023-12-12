//
// Created by Luis Ruisinger on 30.09.23.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/filemanager.h"

uint8_t*
request_file(char* filename)
{
    if (!filename)
        return NULL;

    FILE* file;
    if ((file = fopen(filename, "r")) < 0)
        return NULL;

    fseek(file, 0, SEEK_END);
    uint64_t len = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t* str = calloc(len + 1, sizeof(*str));
    size_t   idx = 0;
    int8_t   chr;

    while((chr = fgetc(file)) != EOF)
        str[idx++] = (uint8_t) chr & 0xFF;

    fclose(file);
    return str;
}