//
// Created by Luis Ruisinger on 04.10.23.
//

#include <string.h>
#include <stdlib.h>
#include "parsing/fieldparser.h"

char* parse_auth_field(char* header, char* field)
{
    char* auth = strstr(header, "Authorization: ");
    if (auth == NULL)
        return NULL;

    auth += strlen("Authorization: ");
    char *end = strchr(auth, '\r');

    if (end != NULL) {
        size_t length = end - auth;
        char *token = (char*) calloc(length + 1, sizeof(char));
        strncpy(token, auth, length);
        return token;
    }
    return NULL;
}

char** parse_fields(char* header)
{
    char* fields[] = {
            "Authorization: ", "Cookie: ", "Accept: "
    };

    uint32_t len = sizeof(fields) / sizeof(*fields);
    char** res = calloc(len, sizeof(char*));

    for (int n = 0; n < len; n++)
        res[n] = parse_auth_field(header, fields[n]);

    return res;
}

bool isfile(char* route)
{
    return strstr(route, ".") != NULL;
}