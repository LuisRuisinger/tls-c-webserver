//
// Created by Luis Ruisinger on 03.10.23.
//

#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>

#include "run.h"
#include "setup.h"
#include "hashmap.h"
#include "tpool.h"
#include "client.h"
#include "tls.h"
#include "requesthandler_arg.h"
#include "requesthandler.h"

#define TIMEOUT 5

void server_run(struct Server* server, struct Hashmap* hashmap)
{
    tpool_t* thread_pool = tpool_create(sysconf(_SC_NPROCESSORS_CONF) + 1);

    struct sockaddr_in6 client_sock_ipv6;
    struct sockaddr_in  client_sock_ipv4;

    socklen_t client_size_ipv6 = sizeof(client_sock_ipv6);
    socklen_t client_size_ipv4 = sizeof(client_sock_ipv4);

    struct timeval timeout = {TIMEOUT, 0};
    fd_set readfds;

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(server->socket, &readfds);

        int32_t waiting = select(server->socket + 1, &readfds, NULL, NULL, &timeout);

        if (waiting == 0)
            continue;

        memset(&client_sock_ipv6, 0, sizeof(client_sock_ipv6));
        memset(&client_sock_ipv4, 0, sizeof(client_sock_ipv4));

        struct Client* client = calloc(1, sizeof(struct Client));

        if (client == NULL)
            continue;

        if (server->protocol == HTTPS)
        {
            if ((client->ctx = create_context()) == NULL)
            {
                free(client);
                continue;
            }

            configure_context(client->ctx);
            client->ssl = SSL_new(client->ctx);
        }

        client->protocol = server->protocol;
        client->fd  = server->version == IPv6
                ? accept(server->socket, (struct sockaddr*) &client_sock_ipv6, &client_size_ipv6)
                : accept(server->socket, (struct sockaddr*) &client_sock_ipv4, &client_size_ipv4);

        uint32_t opt = server->version == IPv6
                ? getsockname(client->fd, (struct sockaddr *) &client_sock_ipv6, &client_size_ipv6)
                : getsockname(client->fd, (struct sockaddr *) &client_sock_ipv4, &client_size_ipv4);

        if (server->protocol == HTTPS)
        {
            if ((client->fd == -1) || (opt == -1))
            {
                SSL_shutdown(client->ssl);
                SSL_free(client->ssl);
                SSL_CTX_free(client->ctx);

                close(client->fd);
                free(client);
                continue;
            }

            SSL_set_fd(client->ssl, client->fd);

            if (SSL_accept(client->ssl) <= 0)
            {
                SSL_shutdown(client->ssl);
                SSL_free(client->ssl);
                SSL_CTX_free(client->ctx);

                close(client->fd);
                free(client);
                continue;
            }
        }
        else if ((client->fd == -1) || (opt == -1))
        {
            close(client->fd);
            free(client);
            continue;
        }

        if (server->version == IPv6)
        {
            client->sin6_addr = client_sock_ipv6.sin6_addr;
            client->sin_port = client_sock_ipv6.sin6_port;
        }
        else {
            client->sin_addr = client_sock_ipv4.sin_addr;
            client->sin_port = client_sock_ipv4.sin_port;
        }

        struct Handler_arg* arg = malloc(sizeof(struct Handler_arg));

        if (arg == NULL)
        {
            if (server->protocol == HTTPS)
            {
                SSL_shutdown(client->ssl);
                SSL_free(client->ssl);
                SSL_CTX_free(client->ctx);
            }
            close(client->fd);
            free(client);
            continue;
        }

        arg->client = client;
        arg->hashmap = hashmap;

        tpool_add_work(thread_pool, (thread_func_t) &request_handler, arg);
    }
}