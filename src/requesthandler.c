//
// Created by Luis Ruisinger on 03.10.23.
//

#include <unistd.h>
#include "requesthandler.h"
#include "requesthandler_arg.h"
#include "readhandler.h"
#include "writehandler.h"

void* request_handler(void* arg)
{
    struct Handler_arg* wrapper = (struct Handler_arg*) arg;

    char* filename = read_client(wrapper->client, wrapper->hashmap);
    if (filename == NULL)
    {
        if (wrapper->client->protocol == HTTPS)
        {
            SSL_shutdown(wrapper->client->ssl);
            SSL_free(wrapper->client->ssl);
            SSL_CTX_free(wrapper->client->ctx);
        }

        close(wrapper->client->fd);
        free(wrapper->client);
        return NULL;
    }

    write_client(wrapper->client, filename);

    if (wrapper->client->protocol == HTTPS)
    {
        SSL_shutdown(wrapper->client->ssl);
        SSL_free(wrapper->client->ssl);
        SSL_CTX_free(wrapper->client->ctx);
    }

    close(wrapper->client->fd);
    free(wrapper->client);
    return NULL;
}