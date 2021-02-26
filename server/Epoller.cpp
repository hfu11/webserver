#include "Epoller.h"

Epoller::Epoller(int maxEvent):epollFd_m(epoll_create(5)),events_m(maxEvent){
    assert(epollFd_m >= 0 && events_m.size() > 0);
}

Epoller::~Epoller(){
    close(epollFd_m);
}

bool Epoller::AddFd(int fd, uint32_t events){
    if(fd < 0) return false;

    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(epollFd_m,EPOLL_CTL_ADD,fd,&ev);
}

bool Epoller::ModFd(int fd, uint32_t events){
    if(fd < 0) return false;

    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(epollFd_m,EPOLL_CTL_MOD,fd,&ev);
}

bool Epoller::DelFd(int fd){
    if(fd < 0) return false;
    epoll_event ev = {0};
    return 0==epoll_ctl(epollFd_m,EPOLL_CTL_DEL,fd,&ev);
}

int Epoller::Wait(int timeoutMs){
    return epoll_wait(epollFd_m,&events_m[0],static_cast<int>(events_m.size()),timeoutMs);
}

int Epoller::GetEventFd(size_t i) const{
    assert(i < events_m.size() && i >= 0);
    return events_m[i].data.fd;
}

uint32_t Epoller::GetEvents(size_t i) const{
    assert(i < events_m.size() && i >= 0);
    return events_m[i].events;
}