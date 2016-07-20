#include "settings.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

void toLog(char *note, char *buf)
{
    FILE *f = fopen("/tmp/final.log", "a");
    fprintf(f, "%s: %s\n", note, buf);
    fclose(f);
}

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
                toLog("IP:   ", optarg);
                settings->ip = optarg;
                break;
            case 'p':
                toLog("Port: ", optarg);
                settings->port = (uint16_t) atoi(optarg);
                break;
            case 'd':
                toLog("Path: ", optarg);
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