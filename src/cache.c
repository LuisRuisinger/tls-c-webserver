//
// Created by Luis Ruisinger on 01.10.23.
//

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/cache.h"
#include "serve.h"

#define MINSIZE 32
#define MIN(x, y) x > y ? y : x

static void cache_put(client* client, time_t* timestamp, struct Cache* cache)
{
    pthread_mutex_lock(&(cache->mutex));
    volatile uint32_t* index = &(cache->index);

    cache->elements[*index] = calloc(1, sizeof(struct Entry));
    if (cache->elements[*index] == NULL)
        exit(EXIT_FAILURE);

    cache->elements[*index]->client = client;
    cache->elements[*index]->timestamp = timestamp;

    *index = (*index + 1) % cache->size;
    pthread_mutex_unlock(&(cache->mutex));
}

static void* cache_update(struct Cache* cache)
{
    struct timeval timeout = {0, 0};
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if (flag)
            break;
        pthread_mutex_unlock(&mutex);

        time_t timestamp = time(NULL);
        for (int n = 0; n < cache->size; n++)
        {
            if (cache->elements[n] == NULL)
                continue;

            fd_set readfds;
            struct Client* client = cache->elements[n]->client;
            time_t* client_time = cache->elements[n]->timestamp;

            FD_ZERO(&readfds);
            FD_SET(client->fd, &readfds);

            if (FD_ISSET(client->fd, &readfds) &&
                (select(client->fd + 1, &readfds, NULL, NULL, &timeout) == 1))
            {
                tpool_add_work(thread_pool, (thread_func_t) &serve, client);

                free(cache->elements[n]->timestamp);
                free((void*) cache->elements[n]);

                cache->elements[n] = NULL;
            }
            else if (difftime(timestamp, *client_time) > 25.0)
            {
                SSL_shutdown(cache->elements[n]->client->ssl);
                SSL_free(cache->elements[n]->client->ssl);
                SSL_CTX_free(cache->elements[n]->client->ctx);

                close(client->fd);

                free(cache->elements[n]->client);
                free(cache->elements[n]->timestamp);
                free((void*) cache->elements[n]);

                cache->elements[n] = NULL;
            }
        }
        sleep(5);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

static void cache_destroy(struct Cache* cache)
{
    for (int n = 0; n < cache->size; n++)
    {
        if (cache->elements[n] == NULL)
            continue;

        SSL_shutdown(cache->elements[n]->client->ssl);
        SSL_free(cache->elements[n]->client->ssl);
        SSL_CTX_free(cache->elements[n]->client->ctx);

        close(cache->elements[n]->client->fd);

        free(cache->elements[n]->client);
        free(cache->elements[n]->timestamp);
        free((void*) cache->elements[n]);

        cache->elements[n] = NULL;
    }

    free(cache->elements);
    pthread_mutex_destroy(&(cache->mutex));
}

cache* cache_init(size_t size)
{
    cache* pool = calloc(1, sizeof(cache));

    if (pool == NULL)
        return NULL;

    pool->put = cache_put;
    pool->update = cache_update;
    pool->destroy = cache_destroy;

    pool->size = MIN(MINSIZE, size);
    pool->index = 0;

    pool->elements = calloc(pool->size, sizeof(struct Entry*));
    if (pool->elements == NULL)
        exit(EXIT_FAILURE);

    for (int n = 0; n < pool->size; n++)
        *(pool->elements + n) = NULL;

    pthread_mutex_init(&(pool->mutex), NULL);
    return pool;
}


