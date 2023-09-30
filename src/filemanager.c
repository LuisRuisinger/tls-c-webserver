//
// Created by Luis Ruisinger on 30.09.23.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/filemanager.h"

char* request_file (char* filename)
{
    char c;
    FILE* file = fopen(filename, "r");

    if (file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END);
    int64_t len = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* temp = malloc(sizeof(char) * (len + 1));

    int32_t i = 0;
    while((c = (char) fgetc(file)) != EOF)
        temp[i++] = c;

    fclose(file);
    return temp;
}