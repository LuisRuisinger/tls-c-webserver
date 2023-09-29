//
// Created by Luis Ruisinger on 29.09.23.
//

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "serve.h"
#include "client.h"
#include "method.h"

#define BUFFER_SIZE 1024

static char resp[] = "HTTP/1.0 200 OK\r\n"
                     "Server: webserver-c\r\n"
                     "Content-type: text/html\r\n\r\n"
                     "<html>hello, gugu</html>\r\n";

static void response(client* cur)
{
    write(cur->fd, resp, strlen(resp));
    close(cur->fd);
}

void* serve(void* args)
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

    switch (*method) {
        default: response(&cur);
    }

    free(buffer);
    free(method);
    free(route);
    free(version);

    close(cur.fd);
    return NULL;
}