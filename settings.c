#include "settings.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

int parseOpts(int argc, char** argv, Settings *settings)
{
    int option;

    if (argc != 7) {
        fprintf(stderr, "Usage: %s -h <ip> -p <port> -d <dir>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((option = getopt(argc, argv, "h:p:d:")) != -1) {
        switch (option) {
            case 'h':
                settings->ip = optarg;
                break;
            case 'p':
                settings->port = (uint16_t) atoi(optarg);
                break;
            case 'd':
                settings->path = optarg;
                break;
            default:
                fprintf(stderr, "Usage: -h <ip> -p <port> -d <directory>\n");
                exit(EXIT_FAILURE);
        }
    }

    int pathLen = strlen(settings->path);
    if (settings->path[pathLen-1] == '/')
        settings->path[pathLen-1] = '\0';

    return 0;
}