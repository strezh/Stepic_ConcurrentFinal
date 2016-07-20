#include <unistd.h>
#include "settings.h"
#include "server.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int main(int argc, char** argv)
{
    Settings settings;
    parseOpts(argc, argv, &settings);

    pid_t pid = fork();

    if(0 > pid) {
        fprintf(stderr, "ERROR: fork failed");
        return EXIT_FAILURE;
    }

    if(0 == pid) {
        signal(SIGCHLD, SIG_IGN);
//        umask(0);
        pid_t sid = setsid();
        chdir(settings.path);

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        pthread_t thread;
        pthread_create(&thread, NULL, runServer, &settings);
        pthread_join(thread, NULL);
    }

    return EXIT_SUCCESS;
}