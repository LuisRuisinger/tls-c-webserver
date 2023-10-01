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
#include "../include/filemanager.h"

#define PORT 8080
#define BACKLOG 16
#define BUFFER_SIZE 1024

pthread_mutex_t mutex;
volatile bool flag;
struct Hashmap* map;

static void clean_mem(char** ptr_list, int32_t len)
{
    for (int n = 0; n < len; n++)
        free(ptr_list[n]);
}

static char* realloc_buffer(char* ptr, int32_t len)
{
    char* new_ptr = calloc(BUFFER_SIZE * len, sizeof(char));
    if (new_ptr == NULL)
    {
        free(ptr);
        return NULL;
    }

    memcpy(new_ptr, ptr, strlen(ptr));
    free(ptr);
    return new_ptr;
}

static char* read_client(client* client)
{
    ssize_t rval;
    ssize_t total = 0;

    char* buffer  = calloc(BUFFER_SIZE, sizeof(char));
    char* method  = calloc(BUFFER_SIZE, sizeof(char));
    char* route   = calloc(BUFFER_SIZE, sizeof(char));
    char* version = calloc(BUFFER_SIZE, sizeof(char));

    if (buffer == NULL || method == NULL || route == NULL || version == NULL)
        return NULL;

    int32_t alloc_read = 1;
    while ((rval = read(client->fd, buffer + total, BUFFER_SIZE * alloc_read - total)) != -1)
    {
        total += rval;
        if (strstr(buffer, "\r\n\r\n") != NULL)
            break;

        if (total >= BUFFER_SIZE * alloc_read)
        {
            buffer = realloc_buffer(buffer, ++alloc_read);
            if (buffer == NULL)
                return NULL;
        }
    }

    if (rval == -1)
    {
        char* arr[] = {method, route, version, buffer};
        clean_mem(arr, 4);
        close(client->fd);
        return NULL;
    }

    if (sscanf(buffer, "%s %s %s", method, route, version) != 3)
    {
        char* arr[] = {method, route, version, buffer};
        clean_mem(arr, 4);
        close(client->fd);
        return NULL;
    }

    fprintf(stdout, "%s %s %s\n", method, version, route);

    char* filename = map->get(route, map);

    char* arr[] = {method, route, version, buffer};
    clean_mem(arr, 4);

    fprintf(stdout, "requesting file : %s\n", filename);
    return filename;
}

static char* write_client(client* client, char* filename)
{
    char* file = request_file(filename);
    char* response  = calloc(BUFFER_SIZE * 4, sizeof(char));

    if (file == NULL)
    {
        if (response != NULL)
            free(response);
        close(client->fd);
        return NULL;
    }

    strcat(response, "HTTP/1.0 200 OK\r\n");
    strcat(response, "Server: webserver-c\r\n");

    if (strstr(filename, ".html"))
        strcat(response, "Content-Type: text/html\r\n");
    else if (strstr(filename, ".js"))
        strcat(response, "Content-Type: application/javascript\r\n");
    else if (strstr(filename, ".css"))
        strcat(response, "Content-Type: text/css\r\n");

    strcat(response, "Content-Length: ");

    char* file_len = calloc(20, sizeof(char));

    if (file_len == NULL)
    {
        free(response);
        close(client->fd);
        return NULL;
    }

    snprintf(file_len, sizeof(file_len), "%lu", strlen(file) + 1);
    strcat(response, file_len);
    free(file_len);

    strcat(response, "\r\n\r\n");
    strcat(response, file);
    strcat(response, "\r\n");

    write(client->fd, response, strlen(response));

    free(response);
    return "";
}

static void* serve(void* args)
{
    char* filename = read_client((client*) args);
    char* res = write_client((client*) args, filename);

    free(args);
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
    map->put("/", "../files/index.html", map);
    map->put("/script.js", "../files/script.js", map);
    map->put("/style.css", "../files/style.css", map);

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

    map->destroy(map);

    free(map);
    free(args);

    return NULL;
}