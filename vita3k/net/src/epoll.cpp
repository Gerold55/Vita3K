#include <net/epoll.h>

int Epoll::add(int id, abs_socket sock, SceNetEpollEvent *ev) {
    if (!eventEntries.try_emplace(id, EpollSocket{ ev->events, ev->data, sock }).second) {
        return SCE_NET_ERROR_EEXIST;
    }
    return 0;
}

int Epoll::del(int id, abs_socket sock, SceNetEpollEvent *ev) {
    return eventEntries.erase(id) == 0 ? SCE_NET_ERROR_ENOENT : 0;
}

int Epoll::mod(int id, abs_socket sock, SceNetEpollEvent *ev) {
    auto it = eventEntries.find(id);
    if (it == eventEntries.end()) return SCE_NET_ERROR_ENOENT;
    it->second.events = ev->events;
    it->second.data = ev->data;
    return 0;
}

int Epoll::wait(SceNetEpollEvent *events, int maxevents, int timeout_microseconds) {
    fd_set readFds, writeFds;
    FD_ZERO(&readFds);
    FD_ZERO(&writeFds);
    int maxFd = 0;

    for (auto &pair : eventEntries) {
        if (pair.second.events & SCE_NET_EPOLLIN) FD_SET(pair.second.sock, &readFds);
        if (pair.second.events & SCE_NET_EPOLLOUT) FD_SET(pair.second.sock, &writeFds);
        if (pair.second.sock > maxFd) maxFd = pair.second.sock;
    }

    timeval timeout { timeout_microseconds / 1000000, timeout_microseconds % 1000000 };
    int ret = select(maxFd + 1, &readFds, &writeFds, nullptr, &timeout);
    if (ret < 0) return -1;

    int eventCount = 0;
    for (auto &pair : eventEntries) {
        unsigned int eventTypes = 0;
        if (FD_ISSET(pair.second.sock, &readFds)) eventTypes |= SCE_NET_EPOLLIN;
        if (FD_ISSET(pair.second.sock, &writeFds)) eventTypes |= SCE_NET_EPOLLOUT;
        if (eventTypes && eventCount < maxevents) {
            events[eventCount++] = { eventTypes, pair.second.data };
        }
    }

    return eventCount;
}
