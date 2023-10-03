//
// Created by Luis Ruisinger on 03.10.23.
//

#ifndef WEBSERVER_C_RUN_H
#define WEBSERVER_C_RUN_H

#include "setup.h"
#include "hashmap.h"

void server_run(struct Server* server, struct Hashmap* hashmap);

#endif //WEBSERVER_C_RUN_H