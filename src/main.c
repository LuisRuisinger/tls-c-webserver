#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "../include/tpool.h"
#include "setup.h"
#include "hashmap.h"
#include "run.h"
#include "method.h"
#include "routetype.h"

int main(int argc, char** argv)
{
    struct Server* server   = server_init(HTTPS, IPv6, "::1", 8080);
    struct Hashmap* hashmap = hashmap_init(16);

    hashmap->add_route("/", "../files/index.html", GET, STATICFILE, hashmap);
    hashmap->add_route("/script.js", "../files/script.js", GET, STATICFILE, hashmap);
    hashmap->add_route("/style.css", "../files/style.css", GET, STATICFILE, hashmap);

    server_run(server, hashmap);
    server_destroy(server);

    return EXIT_SUCCESS;
}
