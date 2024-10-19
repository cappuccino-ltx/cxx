#pragma once

#include <iostream>
#include <sys/epoll.h>

class epoll{
public:
    static const int gsize = 1024;
public:
    static int create() {
        int sock = epoll_create(gsize);
        if (sock < 0) {
            exit(5);
        }
        return sock;
    }
    static bool ctrl(int efd, int oper, int sock, uint32_t events) {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = sock;
        return epoll_ctl(efd, oper, sock, &ev) == 0;
    }
    static int wait(int epfd, struct epoll_event events[], int maxevents, int timeout) {
        return epoll_wait(epfd, events, maxevents, timeout);
    }
};