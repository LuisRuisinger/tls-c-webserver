//
// Created by Luis Ruisinger on 01.10.23.
//

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "../include/client.h"
#include "../include/filemanager.h"
#include "../include/serve.h"

char* write_client(client* client, char* filename)
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