//
// Created by Luis Ruisinger on 29.09.23.
//

#ifndef WEBSERVER_C_METHOD_H
#define WEBSERVER_C_METHOD_H

typedef enum Method
{
    POST,
    GET,
    DELETE,
    PUT,
    HEAD,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH
} method;

#endif //WEBSERVER_C_METHOD_H
