//
// Created by Luis Ruisinger on 28.09.23.
//

#ifndef WEBSERVER_C_CLIENT_H
#define WEBSERVER_C_CLIENT_H

#include <netinet/in.h>
#include <openssl/ssl.h>

#include "setup.h"

typedef struct Client
{
    int32_t fd;
    in_port_t sin_port;
    union
    {
        struct in6_addr sin6_addr;
        struct in_addr sin_addr;
    };
    enum Protocol protocol;
    SSL_CTX* ctx;
    SSL* ssl;
} client;

#endif //WEBSERVER_C_CLIENT_H
