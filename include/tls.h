//
// Created by Luis Ruisinger on 02.10.23.
//

#ifndef WEBSERVER_C_TLS_H
#define WEBSERVER_C_TLS_H

SSL_CTX *create_context();
void configure_context(SSL_CTX *ctx);

#endif //WEBSERVER_C_TLS_H
