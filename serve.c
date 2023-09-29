//
// Created by Luis Ruisinger on 29.09.23.
//

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "serve.h"
#include "client.h"

#define BUFFER_SIZE 4096

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

    char* buffer = calloc(BUFFER_SIZE, sizeof(char));

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

    response(&cur);

    // cach

    free(buffer);
    close(cur.fd);
    return NULL;
}