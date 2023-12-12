#include <stdlib.h>
#include <stdio.h>

#include "setup.h"
#include "hashmaps/hashmap.h"
#include "run.h"

#define TESTIPV6 "::1"
#define TESTIPV4 "127.0.0.1"

int main(int argc, char** argv)
{
    struct Server* server_ipv6 = server_init(HTTP, IPv6, TESTIPV6, 8080);
    struct Hashmap* hashmap    = hashmap_init(16);

    hashmap->add_route("/", "../files/index.html",                     GET, STATICFILE, hashmap);
    hashmap->add_route("/script.js", "../files/script.js",             GET, STATICFILE, hashmap);
    hashmap->add_route("/style.css", "../files/style.css",             GET, STATICFILE, hashmap);
    hashmap->add_route("/notfound", "../files/notfound/notfound.html", GET, STATICFILE, hashmap);

    server_run(server_ipv6, hashmap);
    server_destroy(server_ipv6);

    return EXIT_SUCCESS;
}
