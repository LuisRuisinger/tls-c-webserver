//
// Created by Luis Ruisinger on 29.09.23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#include "../include/tpool.h"
#include "../include/client.h"
#include "../include/serve.h"
#include "../include/hashmap.h"
#include "../include/writehandler.h"
#include "../include/readhandler.h"
#include "tls.h"
#include "cache.h"

struct Cache* pool;

void* serve(void* args)
{
    char* filename = read_client((client*) args, map);
    if (filename == NULL)
    {
        SSL_shutdown(((client*) args)->ssl);
        SSL_free(((client*) args)->ssl);
        SSL_CTX_free(((client*) args)->ctx);
        free(args);
        return NULL;
    }

    char* res = write_client((client*) args, filename);
    time_t* timestamp = calloc(1, sizeof(time_t));

    if ((res == NULL) || (timestamp == NULL))
    {
        SSL_shutdown(((client*) args)->ssl);
        SSL_free(((client*) args)->ssl);
        SSL_CTX_free(((client*) args)->ctx);
        free(args);
        return NULL;
    }

    *timestamp = time(NULL);
    pool->put((client*) args, timestamp, pool);
    return NULL;
}

void* run(int32_t server_fd)
{
    int32_t fd_client;

    struct sockaddr_in6 client_sock;
    struct timeval timeout = {TIMEOUT, 0};

    fd_set readfds;
    socklen_t client_size = sizeof(client_sock);

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);

        pthread_mutex_lock(&mutex);
        if (flag)
            break;
        pthread_mutex_unlock(&mutex);

        if (select(server_fd + 1, &readfds, NULL, NULL, &timeout) == 0)
            continue;

        int32_t pending = select(server_fd + 1, &readfds, NULL, NULL, &timeout);
        for (int32_t n = 0; n < pending; n++)
        {
            memset(&client_sock, 0, sizeof(client_sock));
            client* cur = calloc(1, sizeof(client));

            if ((cur->ctx = create_context()) == NULL)
                continue;

            configure_context(cur->ctx);
            cur->ssl = SSL_new(cur->ctx);

            if (((fd_client = accept(server_fd, (struct sockaddr*) &client_sock, &client_size)) == -1) ||
                ((getsockname(fd_client, (struct sockaddr*) &client_sock, &client_size)) == -1))
            {
                SSL_shutdown(cur->ssl);
                SSL_free(cur->ssl);
                SSL_CTX_free(cur->ctx);
                continue;
            }

            SSL_set_fd(cur->ssl, fd_client);

            if (SSL_accept(cur->ssl) <= 0)
            {
                SSL_shutdown(cur->ssl);
                SSL_free(cur->ssl);
                SSL_CTX_free(cur->ctx);
                continue;
            }

            cur->fd = fd_client;
            cur->sin6_addr = client_sock.sin6_addr;
            cur->sin_port = client_sock.sin6_port;

            tpool_add_work(thread_pool, (thread_func_t) &serve, cur);
        }
    }
    return NULL;
}

void* setup(void* args)
{
    int32_t server_fd;
    int32_t opt = 0;

    struct sockaddr_in6 server_sock;

    map = hashmap_init(16);
    map->add_route("/", "../files/index.html", map);
    map->add_route("/script.js", "../files/script.js", map);
    map->add_route("/style.css", "../files/style.css", map);

    if ((server_fd = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)) == -1)
    {
        fprintf(stderr, "server setsocketopt failed\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    memset(&server_sock, 0, sizeof(server_sock));
    server_sock.sin6_family = AF_INET6;
    server_sock.sin6_port = htons(PORT);

    if ((inet_pton(AF_INET6, (char*) args, &server_sock.sin6_addr) != 1) ||
        (bind(server_fd, (struct sockaddr*) &server_sock, sizeof(server_sock)) == -1) ||
        (listen(server_fd, BACKLOG) == -1))
    {
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    thread_pool = tpool_create(sysconf(_SC_NPROCESSORS_CONF) + 1);

    pool = cache_init(32);
    tpool_add_work(thread_pool,(thread_func_t) (pool->update), pool);

    fprintf(
            stdout,
            "server running on %s : %d with initial route %s pointing to %s\n\n",
            (char*) args,
            ntohs(server_sock.sin6_port),
            "/",
            map->get_route("/", map)
    );

    run(server_fd);
    pthread_mutex_unlock(&mutex);
    tpool_destroy(thread_pool);
    close(server_fd);

    map->destroy(map);
    pool->destroy(pool);

    free(map);
    free(pool);

    return NULL;
}
