//
// Created by Luis Ruisinger on 01.10.23.
//

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "client.h"
#include "filemanager.h"
#include "setup.h"
#include "parsing/parserwrapper.h"

static char* enum_to_string(Code code)
{
    switch (code) {
        case OK:
            return "OK";
        case CREATED:
            return "Created";
        case ACCEPTED:
            return "Accepted";
        case NONAUTHORITATIVEINFORMATION:
            return "Non-Authoritative Information";
        case NOCONTENT:
            return "No Content";
        case RESETCONTENT:
            return "Reset Content";
        case PARTIALCONTENT:
            return "Partial Content";
        case MULTIPLECHOICES:
            return "Multiple Choices";
        case MOVEDPERMANENTLY:
            return "Moved Permanently";
        case FOUND:
            return "Found";
        case SEEOTHER:
            return "See Other";
        case NOTMODIFIED:
            return "Not Modified";
        case USEPROXY:
            return "Use Proxy";
        case TEMPORARYREDIRECT:
            return "Temporary Redirect";
        case BADREQUEST:
            return "Bad Request";
        case UNAUTHORIZED:
            return "Unauthorized";
        case PAYMENTREQUIRED:
            return "Payment Required";
        case FORBIDDEN:
            return "Forbidden";
        case NOTFOUND:
            return "Not Found";
        case METHODNOTALLOWED:
            return "Method Not Allowed";
        case NOTACCEPTABLE:
            return "Not Acceptable";
        case PROXYAUTHENTICATIONREQUIRED:
            return "Proxy Authentication Required";
        case REQUESTTIMEOUT:
            return "Request Timeout";
        case CONFLICT:
            return "Conflict";
        case GONE:
            return "Gone";
        case LENGTHREQUIRED:
            return "Length Required";
        case PRECONDITIONFAILED:
            return "Precondition Failed";
        case REQUESTENTITYTOOLARGE:
            return "Request Entity Too Large";
        case REQUESTURITOOLONG:
            return "Request-URI Too Long";
        case UNSUPPORTEDMEDIATYPE:
            return "Unsupported Media Type";
        case REQUESTEDRANGENOTSATISFIABLE:
            return "Requested Range Not Satisfiable";
        case EXPECTATIONFAILED:
            return "Expectation Failed";
        case INTERNALSERVERERROR:
            return "Internal Server Error";
        case NOTIMPLEMENTED:
            return "Not Implemented";
        case BADGATEWAY:
            return "Bad Gateway";
        case SERVICEUNAVAILABLE:
            return "Service Unavailable";
        case GATEWAYTIMEOUT:
            return "Gateway Timeout";
        case HTTPVERSIONNOTSUPPORTED:
            return "HTTP Version Not Supported";
        default:
            return "Unknown";
    }
}

int32_t write_client(struct Client* client, char* str_body, char* mime, Code code)
{

    char* response = calloc(BUFFER_SIZE * 4, sizeof(char));
    if (str_body == NULL)
    {
        if (response != NULL)
            free(response);
        return EXIT_FAILURE;
    }

    strcat(response, "HTTP/1.0 ");

    char status_code[32];
    snprintf(status_code, sizeof(status_code), "%u", code);
    strcat(response, status_code);

    fprintf(stderr, "status code : %s", status_code);

    strcat(response, " ");
    strcat(response, enum_to_string(code));
    strcat(response, "\r\nServer: webserver-c\r\n");
    strcat(response, "Content-Type: ");
    strcat(response, mime);
    strcat(response, "\r\n");
    strcat(response, "Content-Length: ");

    char file_len[32];
    snprintf(file_len, sizeof(file_len), "%lu", strlen(str_body) + 1);
    strcat(response, file_len);

    strcat(response, "\r\n\r\n");
    strcat(response, str_body);
    strcat(response, "\r\n");

    if (client->protocol == HTTPS)
        SSL_write(client->ssl, response, sizeof(response));
    else {
        write(client->fd, response, sizeof(response));
    }

    free(response);
    return EXIT_SUCCESS;
}
