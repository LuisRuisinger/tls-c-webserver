//
// Created by Luis Ruisinger on 29.09.23.
//

#ifndef WEBSERVER_C_SERVE_H
#define WEBSERVER_C_SERVE_H

#define BUFFER_SIZE 1024
#define PORT 8080
#define BACKLOG 16
#define TIMEOUT 5

pthread_mutex_t mutex;
volatile bool flag;
struct Hashmap* map;

void* setup(void* args);

#endif //WEBSERVER_C_SERVE_H
