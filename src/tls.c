//
// Created by Luis Ruisinger on 02.10.23.
//

#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdlib.h>

#include "../include/tls.h"

SSL_CTX* create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();
    ctx    = SSL_CTX_new(method);

    if (!ctx)
    {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    return ctx;
}

void configure_context(SSL_CTX *ctx)
{
    if (SSL_CTX_use_certificate_file(ctx, "../certificates/cert.pem", SSL_FILETYPE_PEM) <= 0)
    {
        fprintf(stderr, "can't use or open cert.pem\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "../certificates/key.pem", SSL_FILETYPE_PEM) <= 0 )
    {
        fprintf(stderr, "can't use or open cert.pem\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}