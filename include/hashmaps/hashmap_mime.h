//
// Created by Luis Ruisinger on 03.10.23.
//

#ifndef WEBSERVER_C_HASHMAP_MIME_H
#define WEBSERVER_C_HASHMAP_MIME_H

#include "method.h"
#include "routetype.h"

struct Linkedlist;

struct Hashmap_mime
{
    void  (*put)     (char* key, void* value, struct Hashmap_mime* map);
    char* (*get)     (char* key, struct Hashmap_mime* map);
    void  (*destroy) (struct Hashmap_mime* map);

    uint32_t size;
    struct Linkedlist** buckets;
};

struct Hashmap_mime* hashmap_mime_init(size_t size);

#endif //WEBSERVER_C_HASHMAP_MIME_H
