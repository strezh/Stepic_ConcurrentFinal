#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <string.h>
#include <stdint.h>

void toLog(char *note, char *buf);
void toLogLong(char *note, long value);

typedef struct
{
    char *ip;
    uint16_t port;
    char *path;
} Settings;

int parseOpts(int argc, char** argv, Settings *settings);

#endif //PROJECT_SETTINGS_H
