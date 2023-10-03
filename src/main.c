#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "../include/tpool.h"
#include "../include/serve.h"
#include "setup.h"
#include "hashmap.h"
#include "run.h"

int main(int argc, char** argv)
{
    struct Server* server = server_init(HTTP, IPv6, "::1", 8080);

    map = hashmap_init(16);
    map->put("/", "../files/index.html", map);
    map->put("/script.js", "../files/script.js", map);
    map->put("/style.css", "../files/style.css", map);

    server_run(server, map);

    return EXIT_SUCCESS;
}
