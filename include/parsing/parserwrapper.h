//
// Created by Luis Ruisinger on 04.10.23.
//

#ifndef WEBSERVER_C_PARSERWRAPPER_H
#define WEBSERVER_C_PARSERWRAPPER_H

#include "hashmaps/hashmap.h"

enum Code {
    OK, NOTFOUND
};

struct Reqparsestruct
{
    enum Code code;
    bool isfile;
    struct Value* value;
};

#endif //WEBSERVER_C_PARSERWRAPPER_H
