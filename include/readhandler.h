//
// Created by Luis Ruisinger on 01.10.23.
//

#ifndef WEBSERVER_C_READHANDLER_H
#define WEBSERVER_C_READHANDLER_H

#include "client.h"
#include "hashmaps/hashmap.h"

struct Value* read_client(client* client, hashmap* map);

#endif //WEBSERVER_C_READHANDLER_H
