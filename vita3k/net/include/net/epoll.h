#pragma once
#include <unordered_map>
#include <net/socket.h>

struct EpollSocket {
    unsigned int events;
    void* data;
    abs_socket sock;
};

class Epoll {
private:
    std::unordered_map<int, EpollSocket> eventEntries;

public:
    int add(int id, abs_socket sock, SceNetEpollEvent *ev);
    int del(int id, abs_socket sock, SceNetEpollEvent *ev);
    int mod(int id, abs_socket sock, SceNetEpollEvent *ev);
    int wait(SceNetEpollEvent *events, int maxevents, int timeout_microseconds);
};
