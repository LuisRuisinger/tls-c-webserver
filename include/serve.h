//
// Created by Luis Ruisinger on 29.09.23.
//

#ifndef WEBSERVER_C_SERVE_H
#define WEBSERVER_C_SERVE_H

#define BUFFER_SIZE 1024
#define PORT 8080
#define BACKLOG 16
#define TIMEOUT 5

#include "../include/tpool.h"

pthread_mutex_t mutex;
volatile bool flag;
struct Hashmap* map;
tpool_t* thread_pool;

void* serve(void* args);
void* setup(void* args);

#endif //WEBSERVER_C_SERVE_H
