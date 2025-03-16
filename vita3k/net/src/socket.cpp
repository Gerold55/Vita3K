// socket.cpp - Implementation for PosixSocket
#include <net/socket.h>

int PosixSocket::close() {
    #ifdef _WIN32
    return closesocket(sock);
    #else
    return ::close(sock);
    #endif
}

int PosixSocket::bind(const SceNetSockaddr *addr, unsigned int addrlen) {
    return ::bind(sock, reinterpret_cast<const sockaddr *>(addr), addrlen);
}

int PosixSocket::send_packet(const void *msg, unsigned int len, int flags, const SceNetSockaddr *to, unsigned int tolen) {
    return sendto(sock, static_cast<const char *>(msg), len, flags, reinterpret_cast<const sockaddr *>(to), tolen);
}

int PosixSocket::recv_packet(void *buf, unsigned int len, int flags, SceNetSockaddr *from, unsigned int *fromlen) {
    return recvfrom(sock, static_cast<char *>(buf), len, flags, reinterpret_cast<sockaddr *>(from), reinterpret_cast<socklen_t *>(fromlen));
}

int PosixSocket::set_socket_options(int level, int optname, const void *optval, unsigned int optlen) {
    return setsockopt(sock, level, optname, static_cast<const char *>(optval), optlen);
}

int PosixSocket::get_socket_options(int level, int optname, void *optval, unsigned int *optlen) {
    return getsockopt(sock, level, optname, static_cast<char *>(optval), reinterpret_cast<socklen_t *>(optlen));
}

int PosixSocket::connect(const SceNetSockaddr *addr, unsigned int namelen) {
    return ::connect(sock, reinterpret_cast<const sockaddr *>(addr), namelen);
}

SocketPtr PosixSocket::accept(SceNetSockaddr *addr, unsigned int *addrlen) {
    abs_socket newSock = ::accept(sock, reinterpret_cast<sockaddr *>(addr), reinterpret_cast<socklen_t *>(addrlen));
    if (newSock < 0) return nullptr;
    return std::make_shared<PosixSocket>(newSock);
}

int PosixSocket::listen(int backlog) {
    return ::listen(sock, backlog);
}

int PosixSocket::get_socket_address(SceNetSockaddr *name, unsigned int *namelen) {
    return getsockname(sock, reinterpret_cast<sockaddr *>(name), reinterpret_cast<socklen_t *>(namelen));
}