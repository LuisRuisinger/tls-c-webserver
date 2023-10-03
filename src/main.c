#include <stdlib.h>

#include "setup.h"
#include "hashmap.h"
#include "run.h"

#define TESTIPV6 "::1"
#define TESTIPV4 "127.0.0.1"

int main(int argc, char** argv)
{
    struct Server* server_ipv6 = server_init(HTTPS, IPv6, TESTIPV6, 8080);
    struct Server* server_ipv4 = server_init(HTTPS, IPv6, TESTIPV6, 8080);
    struct Hashmap* hashmap    = hashmap_init(16);

    hashmap->add_route("/", "../files/index.html", GET, STATICFILE, hashmap);
    hashmap->add_route("/script.js", "../files/script.js", GET, STATICFILE, hashmap);
    hashmap->add_route("/style.css", "../files/style.css", GET, STATICFILE, hashmap);

    server_run(server_ipv6, hashmap);
    server_run(server_ipv4, hashmap);

    server_destroy(server_ipv6);
    server_destroy(server_ipv4);

    return EXIT_SUCCESS;
}
