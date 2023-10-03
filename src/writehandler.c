//
// Created by Luis Ruisinger on 01.10.23.
//

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "../include/client.h"
#include "../include/filemanager.h"
#include "setup.h"

char* write_client(struct Client* client, char* str_body, char* mime)
{

    char* response = calloc(BUFFER_SIZE * 4, sizeof(char));
    if (str_body == NULL)
    {
        if (response != NULL)
            free(response);
        return NULL;
    }

    strcat(response, "HTTP/1.0 200 OK\r\n");
    strcat(response, "Server: webserver-c\r\n");
    strcat(response, "Content-Type: ");
    strcat(response, mime);
    strcat(response, "\r\n");
    strcat(response, "Content-Length: ");

    char* file_len = calloc(20, sizeof(char));

    if (file_len == NULL)
    {
        free(response);
        close(client->fd);
        return NULL;
    }

    snprintf(file_len, sizeof(file_len), "%lu", strlen(str_body) + 1);
    strcat(response, file_len);
    free(file_len);

    strcat(response, "\r\n\r\n");
    strcat(response, str_body);
    strcat(response, "\r\n");

    if (client->protocol == HTTPS)
        SSL_write(client->ssl, response, (int) strlen(response));
    else {
        write(client->fd, response, (int) strlen(response));
    }

    free(response);

    return "";
}
