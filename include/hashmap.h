//
// Created by Luis Ruisinger on 30.09.23.
//

#ifndef WEBSERVER_C_HASHMAP_H
#define WEBSERVER_C_HASHMAP_H

#include "method.h"
#include "routetype.h"

struct Linkedlist;

struct Value
{
    enum Route_type type;
    union {
        char* route;
        char* (*fun) (char*);
    };
};

typedef struct Hashmap
{
    void          (*add_route) (char* uri, void* value, enum Method method, enum Route_type type, struct Hashmap* map);
    struct Value* (*get_route) (char* uri, struct Hashmap* map);
    void          (*destroy)   (struct Hashmap* map);

    uint32_t size;
    struct Linkedlist** buckets;
} hashmap;

struct Hashmap* hashmap_init(size_t size);

#endif //WEBSERVER_C_HASHMAP_H
