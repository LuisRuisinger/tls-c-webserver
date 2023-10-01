//
// Created by Luis Ruisinger on 01.10.23.
//

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "../include/client.h"
#include "../include/serve.h"
#include "../include/hashmap.h"

static void clean_mem(char** ptr_list, int32_t len)
{
    for (int n = 0; n < len; n++)
        free(ptr_list[n]);
}

static char* realloc_buffer(char* ptr, int32_t len)
{
    char* new_ptr = calloc(BUFFER_SIZE * len, sizeof(char));
    if (new_ptr == NULL)
    {
        free(ptr);
        return NULL;
    }

    memcpy(new_ptr, ptr, strlen(ptr));
    free(ptr);
    return new_ptr;
}

char* read_client(client* client, hashmap* map)
{
    ssize_t rval;
    ssize_t total = 0;

    char* buffer  = calloc(BUFFER_SIZE, sizeof(char));
    char* method  = calloc(BUFFER_SIZE, sizeof(char));
    char* route   = calloc(BUFFER_SIZE, sizeof(char));
    char* version = calloc(BUFFER_SIZE, sizeof(char));

    if (buffer == NULL || method == NULL || route == NULL || version == NULL)
        return NULL;

    int32_t alloc_read = 1;
    while ((rval = read(client->fd, buffer + total, BUFFER_SIZE * alloc_read - total)) != -1)
    {
        total += rval;
        if (strstr(buffer, "\r\n\r\n") != NULL)
            break;

        if (total >= BUFFER_SIZE * alloc_read)
        {
            buffer = realloc_buffer(buffer, ++alloc_read);
            if (buffer == NULL)
                return NULL;
        }
    }

    if (rval == -1)
    {
        char* arr[] = {method, route, version, buffer};
        clean_mem(arr, 4);
        close(client->fd);
        return NULL;
    }

    if (sscanf(buffer, "%s %s %s", method, route, version) != 3)
    {
        char* arr[] = {method, route, version, buffer};
        clean_mem(arr, 4);
        close(client->fd);
        return NULL;
    }

    fprintf(stdout, "%s %s %s\n", method, version, route);

    char* filename = map->get(route, map);

    char* arr[] = {method, route, version, buffer};
    clean_mem(arr, 4);

    fprintf(stdout, "requesting file : %s\n", filename);
    return filename;
}