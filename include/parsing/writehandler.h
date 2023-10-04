//
// Created by Luis Ruisinger on 01.10.23.
//

#ifndef WEBSERVER_C_WRITEHANDLER_H
#define WEBSERVER_C_WRITEHANDLER_H

#include "parserwrapper.h"

int32_t write_client(struct Client* client, char* str_body, char* mime, enum Code code);

#endif //WEBSERVER_C_WRITEHANDLER_H
