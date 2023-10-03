//
// Created by Luis Ruisinger on 30.09.23.
//

#ifndef WEBSERVER_C_HASHMAP_H
#define WEBSERVER_C_HASHMAP_H

struct Linkedlist;

typedef struct Hashmap
{
    void  (*add_route) (char* route, char* file, struct Hashmap* map);
    char* (*get_route) (char* route, struct Hashmap* map);
    void  (*destroy)   (struct Hashmap* map);

    uint32_t size;
    struct Linkedlist** buckets;
} hashmap;

struct Hashmap* hashmap_init(size_t size);

#endif //WEBSERVER_C_HASHMAP_H
