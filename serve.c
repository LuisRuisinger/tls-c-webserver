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

#include "method.h"
#include "tpool.h"
#include "client.h"
#include "serve.h"
#include "hashmap.h"

#define PORT 8080
#define BACKLOG 16
#define BUFFER_SIZE 1024

static char resp[] = "HTTP/1.0 200 OK\r\n"
                     "Server: webserver-c\r\n"
                     "Content-type: text/html\r\n\r\n"
                     "<html>hello, gugu</html>\r\n";

pthread_mutex_t mutex;
volatile bool flag;
struct Hashmap* map;

static void* serve(void* args)
{
    ssize_t rval;
    ssize_t total = 0;

    client cur = *(client*) args;
    free(args);

    char* buffer  = calloc(BUFFER_SIZE * 4, sizeof(char));
    char* method  = calloc(BUFFER_SIZE, sizeof(char));
    char* route   = calloc(BUFFER_SIZE, sizeof(char));
    char* version = calloc(BUFFER_SIZE, sizeof(char));

    if (buffer == NULL || method == NULL || route == NULL || version == NULL)
        return NULL;

    while ((rval = read(cur.fd, buffer + total, BUFFER_SIZE - total)) != -1)
    {
        total += rval;
        if (strstr(buffer, "\r\n\r\n") != NULL)
            break;
    }

    if (rval == -1)
    {
        fprintf(stderr, "read error for client : %d", cur.fd);
        free(buffer);
        close(cur.fd);
        return NULL;
    }

    if (sscanf(buffer, "%s %s %s", method, route, version) != 3)
        return NULL;

    fprintf(stdout, "%s %s %s\n", method, version, route);

    // basic routing

    if (map->get(route, map) != NULL)
    {
        write(cur.fd, resp, strlen(resp));
        close(cur.fd);
    }
    else {
        // return 404 page here
    }

    free(buffer);
    free(method);
    free(route);
    free(version);

    close(cur.fd);
    return NULL;
}

void* run(int32_t server_fd, tpool_t* thread_pool)
{
    int32_t fd_client;

    struct sockaddr_in6 client_sock;
    struct timeval timeout = {5, 0};

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

            if ((fd_client = accept(server_fd, (struct sockaddr*) &client_sock, &client_size)) == -1)
                continue;

            if ((getsockname(fd_client, (struct sockaddr*) &client_sock, &client_size)) == -1)
                continue;

            // freeing this must happen inside the serve function
            // for hardkill and softkill !!!
            client* inf = calloc(1, sizeof(client));

            inf->fd = fd_client;
            inf->sin6_addr = client_sock.sin6_addr;
            inf->sin6_port = client_sock.sin6_port;

            tpool_add_work(thread_pool, (thread_func_t) &serve, inf);
        }
    }
}

void* setup(void* args)
{
    int32_t server_fd;
    int32_t opt = 0;

    struct sockaddr_in6 server_sock;

    tpool_t* thread_pool;

    map = new(16);
    map->put("/", "landing.html", map);
    map->put("/contact", "contact.html", map);
    map->put("/work", "work.html", map);

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

    if (inet_pton(AF_INET6, (char*) args, &server_sock.sin6_addr) != 1)
    {
        fprintf(stderr, "IP parsing failed\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr*) &server_sock, sizeof(server_sock)) == -1)
    {
        fprintf(stderr, "server bind failed\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    thread_pool = tpool_create(sysconf(_SC_NPROCESSORS_CONF) + 1);

    if (listen(server_fd, BACKLOG) == -1)
    {
        fprintf(stderr, "server listen failed\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    fprintf(
            stdout,
            "server running on %s : %d with initial route %s\n",
            (char*) args,
            ntohs(server_sock.sin6_port),
            map->get("/", map)
    );

    run(server_fd, thread_pool);
    pthread_mutex_unlock(&mutex);
    tpool_destroy(thread_pool);
    close(server_fd);

    free(args);

    return NULL;
}