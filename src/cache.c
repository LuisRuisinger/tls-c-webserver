//
// Created by Luis Ruisinger on 01.10.23.
//

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/cache.h"
#include "../include/client.h"

#define MINSIZE 32
#define MIN(x, y) x > y ? y : x

static uint32_t cache_put(client* client, time_t timestamp, struct Cache* cache)
{
    return 0;
}

static void cache_update(struct Cache* cache)
{

}

static void cache_destroy(struct Cache* cache)
{

}

cache* new(size_t size)
{
    cache* store = calloc(1, sizeof(cache));

    if (store == NULL)
        return NULL;

    store->put = cache_put;
    store->update = cache_update;
    store->destroy = cache_destroy;

    store->size = MIN(MINSIZE, size);
    store->elements = calloc(store->size, sizeof(client*));

    for (int n = 0; n < store->size; n++)
        *(store->elements + n) = NULL;

    return store;
}


