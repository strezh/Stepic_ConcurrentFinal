//
// Created by user on 7/18/16.
//

#include "server.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <errno.h>
#include <malloc.h>

#define EPOLL_SIZE  10000
#define BUF_LEN     1024

static char Resp_200[] = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
static char Resp_404[] = "HTTP/1.0 404 Not found\r\n\r\n";

int setNonblock(int fd)
{
    int fdflags = fcntl(fd, F_GETFL, 0);
    if (fdflags == -1)
        return -1;
    fdflags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, fdflags) == -1)
        return -1;
    return 0;
}


void *runServer(void* value)
{
    int i = 0;

    Settings *settings = (Settings *)value;
    struct sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(settings->port);

    int master = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (0 >= inet_pton(AF_INET, settings->ip, &sockAddr.sin_addr.s_addr))
        fprintf(stderr,"ERROR: wrong IP address\n");

    bind(master, (struct sockaddr *)(&sockAddr), sizeof(sockAddr));
    setNonblock(master);
    listen(master, SOMAXCONN);

    int epoll = epoll_create1(0);

    struct epoll_event epollEvent;
    epollEvent.data.fd = master;
    epollEvent.events = EPOLLIN;
    epoll_ctl(epoll, EPOLL_CTL_ADD, master, &epollEvent);

    while(1) {
        struct epoll_event events[EPOLL_SIZE];

        int numEvents = epoll_wait(epoll, events, EPOLL_SIZE, -1);

        for (i = 0; i < numEvents; ++i) {
            if (events[i].data.fd == master) {
                int slave = accept (master, 0, 0);
                struct epoll_event event;
                event.data.fd = slave;
                event.events = EPOLLIN;

                setNonblock (slave);
                epoll_ctl(epoll, EPOLL_CTL_ADD, slave, &event);
            } else {
                char query[BUF_LEN];
                memset(query, 0, sizeof(query));
                if (0 < recv(events[i].data.fd, query, BUF_LEN, MSG_NOSIGNAL)) {
                    int start = 0;
                    int queryLen = strlen(query);

                    toLog("query src:   ", query);

                    while((query[start] != '/') && (start < queryLen))
                        start++;

                    int finish = start;
                    while((query[finish] != ' ')
                          && (query[finish] != '?')
                          && (finish < queryLen))
                        finish++;

                    char queryPath[BUF_LEN];
                    memset(queryPath, 0, sizeof(queryPath));
                    strcat(queryPath, settings->path);

                    int pathLen = strlen(queryPath);
                    strncpy(queryPath + pathLen, query+start, finish-start);
                    queryPath[pathLen + finish-start] = '\0';

                    toLog("queryPath: ", queryPath);

                    FILE *f = fopen(queryPath, "r");
                    struct stat statbuf;
                    stat(queryPath, &statbuf);

                    if((f != NULL) && !S_ISDIR(statbuf.st_mode)) {
                        toLog("TRY to read:", queryPath);

                        char *buf;
                        size_t len;
                        fseek(f, 0, SEEK_END);
                        len = ftell(f);
                        int respLen = strlen(Resp_200);
                        buf = (char *) malloc(len + respLen + 1);
                        memset(buf, 0, sizeof(len + respLen));

                        strcpy(buf, Resp_200);
                        fseek(f, 0, SEEK_SET);
                        fread(buf + respLen, 1, len, f);
                        fclose(f);

                        buf[len + respLen] = '\0';

                        toLog("buf send: ", buf);

                        send(events[i].data.fd, buf, strlen(buf), MSG_NOSIGNAL);
                    } else {
                        toLog("WRONG: ", Resp_404);
                        send(events[i].data.fd, Resp_404, strlen(Resp_404), MSG_NOSIGNAL);
                    }

                    shutdown(events[i].data.fd, SHUT_RDWR);
                    close(events[i].data.fd);
                } else if (errno != EAGAIN) {
                    shutdown(events[i].data.fd, SHUT_RDWR);
                    close(events[i].data.fd);
                }
            }
        }
    }

}


