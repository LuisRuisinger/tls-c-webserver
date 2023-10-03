//
// Created by Luis Ruisinger on 03.10.23.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "hashmaps/hashmap_mime.h"
#include "routetype.h"
#include "method.h"
#include "filemanager.h"

#define MINSIZE 4
#define MIN(x, y) x > y ? y : x

typedef struct Linkedlist
{
    char* key;
    char* value;
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

static void map_put(
        char* key,
        void* value,
        struct Hashmap_mime* map)
{
    uint32_t hash = fnv1a_hash(key);
    uint32_t index = hash % map->size;


    if (map->buckets[index] == NULL)
    {
        map->buckets[index] = malloc(sizeof(struct Linkedlist));

        if (map->buckets[index] == NULL)
        {
            fprintf(stderr, "failed to add uri");
            exit(EXIT_FAILURE);
        }

        map->buckets[index]->key   = strdup(key);
        map->buckets[index]->value = strdup(value);
        map->buckets[index]->next  = NULL;
    }
    else {
        struct Linkedlist* cur = map->buckets[index];
        while (cur->next != NULL)
            cur = cur->next;

        cur->next = malloc(sizeof(struct Linkedlist));
        cur = cur->next;

        if (cur == NULL)
        {
            fprintf(stderr, "failed to add uri");
            exit(EXIT_FAILURE);
        }

        cur->key   = strdup(key);
        cur->value = strdup(value);
        cur->next  = NULL;
    }
}

static char* map_get(char* key, struct Hashmap_mime* map)
{
    uint32_t hash  = fnv1a_hash(key);
    uint32_t index = hash % map->size;

    list* cur = map->buckets[index];
    while (cur != NULL)
    {
        char* cur_key = (cur->key);
        if (strlen(cur_key) == strlen(key) &&
            memcmp(cur_key, key, strlen(key)) == 0)
            return cur->value;
        cur = cur->next;
    }
    return NULL;
}

static void map_destroy(struct Hashmap_mime* map)
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

struct Hashmap_mime* hashmap_mime_init(size_t size)
{
    struct Hashmap_mime* map = calloc(1, sizeof(struct Hashmap_mime));

    if (map == NULL)
        return NULL;

    map->put     = map_put;
    map->get     = map_get;
    map->destroy = map_destroy;

    map->size    = MIN(MINSIZE, size);
    map->buckets = calloc(map->size, sizeof(list*));

    for (int n = 0; n < map->size; n++)
        *(map->buckets + n) = NULL;

    return map;
}