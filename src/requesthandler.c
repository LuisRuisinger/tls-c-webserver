//
// Created by Luis Ruisinger on 03.10.23.
//

#include <unistd.h>
#include "requesthandler.h"
#include "requesthandler_arg.h"
#include "parsing/readhandler.h"
#include "parsing/writehandler.h"
#include "hashmaps/hashmap.h"
#include "parsing/parserwrapper.h"
#include "filemanager.h"


void* request_handler(void* arg)
{
    struct Handler_arg* wrapper    = (struct Handler_arg*) arg;
    struct Reqparsestruct* pstruct = read_client(wrapper->client, wrapper->hashmap);

    if (pstruct == NULL)
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

    struct Value* value = pstruct->value;

    if (pstruct->code == NOTFOUND && pstruct->isfile)
        write_client(
                wrapper->client,
                value->fun(value->route),
                value->mime,
                pstruct->code
        );
    else if (pstruct->code == NOTFOUND && !pstruct->isfile)
        write_client(
                wrapper->client,
                "{\n"
                "  \"error\": \"Resource not found\",\n"
                "  \"message\": \"The requested resource does not exist.\"\n"
                "}\n",
                "application/json",
                pstruct->code
        );
    else {

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
                value->mime,
                pstruct->code
        );
    }


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