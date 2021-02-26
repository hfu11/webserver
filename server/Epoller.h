#ifndef EPOLLER_H
#define EPOLLER_H

#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <vector>
#include <assert.h>

class Epoller
{
public:
    explicit Epoller(int maxEvent = 1024);
    ~Epoller();

    bool AddFd(int fd, uint32_t events);

    bool ModFd(int fd, uint32_t events);

    bool DelFd(int fd);

    int Wait(int timeoutMs = -1);

    int GetEventFd(size_t i) const;

    uint32_t GetEvents(size_t i) const;

    int epollFd_m;

    std::vector<epoll_event> events_m;

};

#endif