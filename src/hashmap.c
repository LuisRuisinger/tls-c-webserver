//
// Created by Luis Ruisinger on 30.09.23.
//

#include <string.h>
#include <stdlib.h>

#include "../include/hashmap.h"

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
        char* cur_route = (cur->route);
        if (strlen(cur_route) == strlen(route) &&
            memcmp(cur_route, route, strlen(route)) == 0)
            return cur->file;
        cur = cur->next;
    }
    return NULL;
}

static void map_destroy(struct Hashmap* map)
{
    for (int n = 0; n < map->size; n++)
    {
        list* cur = map->buckets[n];
        while (cur != NULL)
        {
            list* ptr = cur;
            cur = cur->next;
            free(ptr);
        }
    }
}

hashmap* new(size_t size)
{
    hashmap* map = calloc(1, sizeof(hashmap));

    if (map == NULL)
        return NULL;

    map->put = map_put;
    map->remove = map_remove;
    map->get = map_get;
    map->destroy = map_destroy;

    map->size = MIN(MINSIZE, size);
    map->buckets = calloc(map->size, sizeof(list*));

    for (int n = 0; n < map->size; n++)
        *(map->buckets + n) = NULL;

    return map;
}
