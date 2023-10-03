//
// Created by Luis Ruisinger on 30.09.23.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

static void map_put(char* route, char* file, struct Hashmap* map)
{
    uint32_t hash = fnv1a_hash(route);
    uint32_t index = hash % map->size;

    if (map->buckets[index] == NULL)
    {
        map->buckets[index] = calloc(1, sizeof(list));

        if (map->buckets[index] == NULL)
        {
            fprintf(stderr, "failed to add route");
            exit(EXIT_FAILURE);
        }

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
        {
            fprintf(stderr, "failed to add route");
            exit(EXIT_FAILURE);
        }

        cur->route = strdup(route);
        cur->file = strdup(file);
        cur->next = NULL;
    }
}

static char* map_get(char* route, struct Hashmap* map)
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

hashmap* hashmap_init(size_t size)
{
    hashmap* map = calloc(1, sizeof(hashmap));

    if (map == NULL)
        return NULL;

    map->add_route = map_put;
    map->get_route = map_get;
    map->destroy = map_destroy;

    map->size = MIN(MINSIZE, size);
    map->buckets = calloc(map->size, sizeof(list*));

    for (int n = 0; n < map->size; n++)
        *(map->buckets + n) = NULL;

    return map;
}
