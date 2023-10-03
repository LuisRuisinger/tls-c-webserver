//
// Created by Luis Ruisinger on 30.09.23.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "hashmaps/hashmap.h"
#include "hashmaps/hashmap_mime.h"
#include "routetype.h"
#include "method.h"
#include "filemanager.h"

#define MINSIZE 4
#define MIN(x, y) x > y ? y : x

typedef struct Linkedlist
{
    char* uri;
    struct Value* value;
    enum Method method;
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

static char* find_file_ending(char* route)
{
    char* delimiter = ".";
    char* last_occ  = NULL;
    char* cur_pos   = route;

    while ((cur_pos = strstr(cur_pos, delimiter)) != NULL) {
        last_occ = cur_pos;
        cur_pos += strlen(delimiter); // Move past the last_occ occurrence
    }

    return strdup(++last_occ);
}

static void map_put(
        char* uri,
        void* value,
        enum Method method,
        enum Route_type type,
        struct Hashmap* map)
{
    uint32_t hash = fnv1a_hash(uri);
    uint32_t index = hash % map->size;

    char* response;

    if (method == STATICFILE)
    {
        char* file_ending = find_file_ending((char*) value);

        if (file_ending == NULL)
        {
            fprintf(stderr, "trying to add static file without file ending");
            exit(EXIT_FAILURE);
        }

        response = map->mime->get(file_ending, map->mime);
        free(file_ending);
    }
    else {
        response = strdup("application/json");
    }

    if (map->buckets[index] == NULL)
    {
        map->buckets[index] = malloc(sizeof(struct Linkedlist));

        if (map->buckets[index] == NULL)
        {
            fprintf(stderr, "failed to add uri");
            exit(EXIT_FAILURE);
        }

        map->buckets[index]->uri    = strdup(uri);
        map->buckets[index]->method = method;
        map->buckets[index]->next   = NULL;
        map->buckets[index]->value  = malloc(sizeof(struct Value));

        if (map->buckets[index]->value == NULL)
        {
            fprintf(stderr, "failed to add uri");
            exit(EXIT_FAILURE);
        }

        map->buckets[index]->value->type = type;
        map->buckets[index]->value->mime = response;

        if (type == STATICFILE)
        {
            map->buckets[index]->value->route = strdup((char*) value);
            map->buckets[index]->value->fun   = &request_file;
        }
        else {
            map->buckets[index]->value->fun = value;
        }
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

        cur->uri    = strdup(uri);
        cur->method = method;
        cur->next   = NULL;
        cur->value  = malloc(sizeof(struct Value));

        if (cur->value == NULL)
        {
            fprintf(stderr, "failed to add uri");
            exit(EXIT_FAILURE);
        }

        cur->value->type = type;
        cur->value->mime = response;

        if (type == STATICFILE)
        {
            cur->value->route = strdup((char*) value);
            cur->value->fun   = &request_file;
        }
        else {
            cur->value->fun = value;
        }
    }
}

static struct Value* map_get(char* route, struct Hashmap* map)
{
    uint32_t hash  = fnv1a_hash(route);
    uint32_t index = hash % map->size;

    list* cur = map->buckets[index];
    while (cur != NULL)
    {
        char* cur_route = (cur->uri);
        if (strlen(cur_route) == strlen(route) &&
            memcmp(cur_route, route, strlen(route)) == 0)
            return cur->value;
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
    map->destroy   = map_destroy;

    map->size      = MIN(MINSIZE, size);
    map->buckets   = calloc(map->size, sizeof(struct Linkedlist*));
    map->mime      = hashmap_mime_init(16);

    if (map->buckets == NULL || map->mime == NULL)
    {
        fprintf(stderr, "error allocating memory for members of hashmap");
        exit(EXIT_FAILURE);
    }

    for (int n = 0; n < map->size; n++)
        *(map->buckets + n) = NULL;

    return map;
}
