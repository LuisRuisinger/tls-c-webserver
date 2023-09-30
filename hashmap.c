//
// Created by Luis Ruisinger on 30.09.23.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "hashmap.h"

#define MINSIZE 4
#define MIN(x, y) x > y ? y : x

typedef struct Linkedlist
{
    char* route;
    char* file;

    struct Linkedlist* next;
} list;

static uint32_t fnv1a_hash(const char *str)
{
    uint32_t hash = 2166136261u;
    while (*str)
    {
        hash ^= (uint32_t)*str++;
        hash *= 16777619u;
    }
    return hash;
}

static uint32_t map_put (char* route, char* file, struct Hashmap* map)
{
    uint32_t hash = fnv1a_hash(route);
    uint32_t index = hash % map->size;

    if (map->buckets[index] == NULL)
    {
        map->buckets[index] = calloc(1, sizeof(list));

        if (map->buckets[index] == NULL)
            return 1;

        map->buckets[index]->route = strdup(route);
        map->buckets[index]->file = strdup(file);
        map->buckets[index]->next = NULL;
    }
    else {
        list* cur = map->buckets[index];
        while (cur->next != NULL)
            cur = cur->next;

        cur->next = calloc(1, sizeof(list));
        cur = cur->next;

        if (cur == NULL)
            return 1;

        cur->route = strdup(route);
        cur->file = strdup(file);
        cur->next = NULL;
    }
    return 0;
}

static uint32_t map_remove (char* route, struct Hashmap* map)
{
    uint32_t hash = fnv1a_hash(route);
    uint32_t index = hash % map->size;

    list* cur = map->buckets[index];
    list* bef = NULL;

    while (cur != NULL)
    {
        if (*(cur->route) == *route)
        {
            if (bef != NULL)
            {
                bef->next = cur->next;
                free(cur);
                return 0;
            }

            free(cur);
            map->buckets[index] = NULL;
            return 0;
        }
        bef = cur;
        cur = cur->next;
    }

    return 1;
}

static char* map_get (char* route, struct Hashmap* map)
{
    uint32_t hash = fnv1a_hash(route);
    uint32_t index = hash % map->size;

    list* cur = map->buckets[index];
    while (cur != NULL)
    {
        if (*(cur->route) == *route)
            return cur->file;
        cur = cur->next;
    }
    return NULL;
}

hashmap* new(size_t size)
{
    hashmap* map = calloc(1, sizeof(hashmap));

    if (map == NULL)
        return NULL;

    map->put = map_put;
    map->remove = map_remove;
    map->get = map_get;
    map->size = MIN(MINSIZE, size);

    fprintf(stdout, "a new map has been made with size : %d", map->size);

    return map;
}
