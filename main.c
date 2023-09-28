#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#include "tpool.h"
#include "client.h"

pthread_mutex_t mutex;
bool flag;

static void* serve(void* args) {}

static void* run(void* args)
{
    int32_t server_fd;
    int32_t fd_client;

    struct sockaddr_in6 server_sock;
    struct sockaddr_in6 client_sock;
    tpool_t* thread_pool;

    socklen_t client_size = sizeof(client_sock);

    if ((server_fd = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set the socket option to allow both IPv4 and IPv6 connections
    int opt = 0;
    if (setsockopt(server_fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    memset(&server_sock, 0, sizeof(server_sock));
    server_sock.sin6_family = AF_INET6;
    server_sock.sin6_port = htons(80);

    // using the ip in args
    if (inet_pton(AF_INET, (char*) args, &server_sock.sin6_addr) != 1)
    {
        perror("ip parsing failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr*)&server_sock, sizeof(server_sock)) == -1)
    {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // creates a thread pool with the size: amount cores + 1
    thread_pool = tpool_create(sysconf(_SC_NPROCESSORS_CONF) + 1);
    listen(server_fd, 16);

    fprintf(stdout, "--- server online ---\n");
    fprintf(stdout, "--- %s ---\n", (char*) args);

    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (flag)
            break;
        pthread_mutex_unlock(&mutex);

        // reseting client struct
        memset(&client_sock, 0, sizeof(client_sock));
        fd_client = accept(server_fd, (struct sockaddr *) &client_sock, &client_size);

        // freeing this must happen inside the serve function
        // for hardkill and softkill !!!
        client* inf = calloc(1, sizeof(client));

        inf->fd = fd_client;
        inf->sin6_addr = client_sock.sin6_addr;
        inf->sin6_port = client_sock.sin6_port;

        tpool_add_work(thread_pool, (thread_func_t) &serve, inf);
    }

    pthread_mutex_unlock(&mutex);

    tpool_destroy(thread_pool);
    close(server_fd);

    return NULL;
}

int main(int argc, char** argv)
{
    char input[256];

    if (argc != 2)
    {
        perror("invalid amount of args");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "--- starting server ---\n");

    pthread_mutex_init(&mutex, NULL);
    flag = false;

    pthread_t runner;
    char* args = calloc(strlen(argv[1]) + 1, sizeof(char));
    strcpy(args, argv[1]);

    pthread_create(&runner, NULL, run, args);

    while (1)
        if (fgets(input, sizeof(input), stdin) != NULL && strcmp(input, "kill\n") == 0)
        {
            pthread_mutex_lock(&mutex);
            flag = true;
            pthread_mutex_unlock(&mutex);
            break;
        }

    pthread_join(runner, NULL);
    pthread_mutex_destroy(&mutex);

    free(args);

    return EXIT_SUCCESS;
}
