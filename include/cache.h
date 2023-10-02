//
// Created by Luis Ruisinger on 01.10.23.
//

#ifndef WEBSERVER_C_CACHE_H
#define WEBSERVER_C_CACHE_H

#include "client.h"

struct Entry
{
    struct Client* client;
    time_t* timestamp;
};

typedef struct Cache
{
    void  (*put)     (client* client, time_t* timestamp, struct Cache* cache);
    void* (*update)  (struct Cache* cache);
    void  (*destroy) (struct Cache* cache);

    uint32_t size;
    volatile uint32_t index;
    volatile struct Entry** elements;
    pthread_mutex_t mutex;
} cache;

cache* cache_init(size_t size);

#endif //WEBSERVER_C_CACHE_H
