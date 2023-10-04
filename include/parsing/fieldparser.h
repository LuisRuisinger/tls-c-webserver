//
// Created by Luis Ruisinger on 04.10.23.
//

#ifndef WEBSERVER_C_FIELDPARSER_H
#define WEBSERVER_C_FIELDPARSER_H

char* parse_auth_field(char* header, char* field);
char** parse_fields(char* header);
bool isfile(char* route);

#endif //WEBSERVER_C_FIELDPARSER_H
