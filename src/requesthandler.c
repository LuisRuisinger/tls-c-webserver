//
// Created by Luis Ruisinger on 03.10.23.
//

#include <unistd.h>
#include "requesthandler.h"
#include "requesthandler_arg.h"
#include "readhandler.h"
#include "writehandler.h"
#include "hashmaps/hashmap.h"

void* request_handler(void* arg)
{
    struct Handler_arg* wrapper = (struct Handler_arg*) arg;
    struct Value* value         = read_client(wrapper->client, wrapper->hashmap);

    if (value->type == STATICFILE && value->route == NULL)
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

    write_client(
            wrapper->client,
            value->fun(value->route),
            value->mime
    );

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