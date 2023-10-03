//
// Created by Luis Ruisinger on 03.10.23.
//

#ifndef WEBSERVER_C_REQUESTHANDLER_ARG_H
#define WEBSERVER_C_REQUESTHANDLER_ARG_H

#include "client.h"

struct Handler_arg {
    struct Client* client;
    struct Hashmap* hashmap;
};

#endif //WEBSERVER_C_REQUESTHANDLER_ARG_H
