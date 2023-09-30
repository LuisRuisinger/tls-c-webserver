//
// Created by Luis Ruisinger on 29.09.23.
//

#ifndef WEBSERVER_C_SERVE_H
#define WEBSERVER_C_SERVE_H

pthread_mutex_t mutex;
volatile bool flag;
struct Hashmap* map;

void* run(void* args);

#endif //WEBSERVER_C_SERVE_H
