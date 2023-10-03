//
// Created by Luis Ruisinger on 03.10.23.
//

#ifndef WEBSERVER_C_SETUP_H
#define WEBSERVER_C_SETUP_H

#define BUFFER_SIZE 1024
#define BACKLOG 16
#define TIMEOUT 5

enum Protocol {
    HTTP, HTTPS
};

enum Version {
    IPv4, IPv6
};

struct Server {
    int32_t socket;
    int32_t port;
    enum Version version;
    enum Protocol protocol;
};

struct Server* server_init(enum Protocol protocol, enum Version version, char* ip, int32_t port);
void server_destroy(struct Server* server);

#endif //WEBSERVER_C_SETUP_H
