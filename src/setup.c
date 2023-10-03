//
// Created by Luis Ruisinger on 03.10.23.
//

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "setup.h"

struct Server* server_init(
        enum Protocol protocol,
        enum Version version,
        char* ip,
        int32_t port
){
    int32_t opt = 0;
    struct Server* server = malloc(sizeof(struct Server));

    if (server == NULL)
    {
        fprintf(stderr, "insufficient memory");
        exit(EXIT_FAILURE);
    }

    server->version  = version;
    server->protocol = protocol;
    server->port     = port;
    server->socket   = version == IPv6
            ? socket(AF_INET6, SOCK_STREAM, 0)
            : socket(AF_INET, SOCK_STREAM, 0);

    if (server->socket == -1)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (server->version == IPv6)
    {
        struct sockaddr_in6 sock_addr;
        if (setsockopt(server->socket, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)) == -1)
        {
            fprintf(stderr, "server setsocketopt for IPv6 failed");
            close(server->socket);
            exit(EXIT_FAILURE);
        }

        memset(&sock_addr, 0, sizeof(sock_addr));

        sock_addr.sin6_family = AF_INET6;
        sock_addr.sin6_port   = htons(server->port);

        if (ip == NULL)
            sock_addr.sin6_addr = in6addr_any;
        else if (inet_pton(AF_INET6, ip, &sock_addr.sin6_addr) != 1)
        {
            close(server->socket);
            exit(EXIT_FAILURE);
        }

        int32_t bin = bind(server->socket, (struct sockaddr*) &sock_addr, sizeof(sock_addr));
        int32_t lis = listen(server->socket, BACKLOG);

        if ((bin == -1) || (lis == -1))
        {
            close(server->socket);
            exit(EXIT_FAILURE);
        }
    }
    else {
        struct sockaddr_in sock_addr;
        if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        {
            fprintf(stderr, "server setsockopt for IPv4 failed");
            close(server->socket);
            exit(EXIT_FAILURE);
        }

        memset(&sock_addr, 0, sizeof(sock_addr));

        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port   = htons(server->port);

        if (ip == NULL)
            sock_addr.sin_addr.s_addr = INADDR_ANY;
        else if (inet_pton(AF_INET, ip, &(sock_addr.sin_addr)) != 1)
        {
            close(server->socket);
            exit(EXIT_FAILURE);
        }

        int32_t bin = bind(server->socket, (struct sockaddr*) &sock_addr, sizeof(sock_addr));
        int32_t lis = listen(server->socket, BACKLOG);

        if ((bin == -1) || (lis == -1))
        {
            close(server->socket);
            exit(EXIT_FAILURE);
        }
    }
    return server;
}

void server_destroy(struct Server* server)
{
    close(server->socket);
    free(server);
}

