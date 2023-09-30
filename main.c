#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "tpool.h"
#include "serve.h"
#include "hashmap.h"

#define PORT 8080
#define BACKLOG 16

int main(int argc, char** argv)
{
    char input[256];

    if (argc != 2)
    {
        perror("invalid amount of args");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&mutex, NULL);
    flag = false;

    fprintf(stdout, "--- starting server ---\n");

    pthread_t runner;
    char* args = calloc(strlen(argv[1]) + 1, sizeof(char));
    strcpy(args, argv[1]);

    pthread_create(&runner, NULL, setup, args);

    while (1)
        if (fgets(input, sizeof(input), stdin) != NULL && strcmp(input, "kill\n") == 0)
        {
            pthread_mutex_lock(&mutex);
            flag = true;
            pthread_mutex_unlock(&mutex);
            break;
        }

    pthread_join(runner, NULL);
    pthread_mutex_destroy(&mutex);

    free(args);

    return EXIT_SUCCESS;
}
