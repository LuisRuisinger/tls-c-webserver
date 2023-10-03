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

    map->put("html", "text/html", map);
    map->put("js", "application/javascript", map);
    map->put("css", "text/css", map);
    map->put("jpg", "image/jpeg", map);
    map->put("jpeg", "image/jpeg", map);
    map->put("png", "image/png", map);
    map->put("gif", "image/gif", map);
    map->put("bmp", "image/bmp", map);
    map->put("ico", "image/x-icon", map);
    map->put("svg", "image/svg+xml", map);
    map->put("pdf", "application/pdf", map);
    map->put("doc", "application/msword", map);
    map->put("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document", map);
    map->put("xls", "application/vnd.ms-excel", map);
    map->put("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", map);
    map->put("ppt", "application/vnd.ms-powerpoint", map);
    map->put("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation", map);
    map->put("zip", "application/zip", map);
    map->put("rar", "application/x-rar-compressed", map);
    map->put("tar", "application/x-tar", map);
    map->put("gz", "application/gzip", map);
    map->put("mp3", "audio/mpeg", map);
    map->put("ogg", "audio/ogg", map);
    map->put("wav", "audio/wav", map);
    map->put("mp4", "video/mp4", map);
    map->put("avi", "video/x-msvideo", map);
    map->put("glsl", "application/x-glsl", map);
    map->put("vert", "application/x-glsl", map);
    map->put("frag", "application/x-glsl", map);
    map->put("geom", "application/x-glsl", map);
    map->put("comp", "application/x-glsl", map);
    map->put("hlsl", "application/x-hlsl", map);
    map->put("cg", "application/x-cg", map);
    map->put("obj", "model/obj", map);
    map->put("fbx", "model/fbx", map);
    map->put("gltf", "model/gltf+json", map);
    map->put("glb", "model/gltf-binary", map);
    map->put("dae", "model/vnd.collada+xml", map);
    map->put("stl", "model/stl", map);
    map->put("ply", "model/ply", map);
    map->put("3ds", "image/x-3ds", map);
    map->put("blend", "application/x-blender", map);
    map->put("x3d", "model/x3d+xml", map);
    map->put("mtl", "text/plain", map);
    map->put("shader", "application/x-shader", map);
    map->put("spv", "application/x-shader-vertex", map);
    map->put("spf", "application/x-shader-fragment", map);
    map->put("sph", "application/x-shader-geometry", map);
    map->put("spt", "application/x-shader-tessellation", map);
    map->put("spp", "application/x-shader-compute", map);
    map->put("jsx", "text/jsx", map);
    map->put("scss", "text/x-scss", map);
    map->put("less", "text/less", map);
    map->put("vue", "text/vue", map);
    map->put("json", "application/json", map);

    return map;
}