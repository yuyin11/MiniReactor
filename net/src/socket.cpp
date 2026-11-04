
#include "../include/reactor/net/socket.h"

#include <asm-generic/socket.h>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::~Socket() {
  if (fd_ >= 0)
    ::close(fd_);
}

void Socket::setNonBlocking(bool on) {
  int flags = ::fcntl(fd_, F_GETFL, 0);
  if (on)
    flags |= O_NONBLOCK;
  else
    flags &= O_NONBLOCK;
  ::fcntl(fd_, F_SETFL, flags);
}

void Socket::setReuseAddr(bool on) {
  int val = on ? 1 : 0;
  // optval: 1(enable), 0(disable)
  ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
}

void Socket::setReusePort(bool on) {
  int val = on ? 1 : 0;
  ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
}

void Socket::bind(sockaddr_in const &addr) {
  int ret =
      ::bind(fd_, reinterpret_cast<sockaddr const *>(&addr), sizeof(addr));
  assert(ret == 0);
}

void Socket::listen() {
  // SOMAXCONN: Socket Maximum Connections(4096)
  int ret = ::listen(fd_, SOMAXCONN);
  assert(ret == 0);
}

int Socket::accept(sockaddr_in *peerAddr) {
  sockaddr_in addr;
  socklen_t len = sizeof(addr);
  std::memset(&addr, 0, sizeof(addr));

  int connfd = ::accept4(fd_, reinterpret_cast<sockaddr *>(&addr), &len,
                         SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (connfd >= 0) {
    if (peerAddr)
      *peerAddr = addr;
  } else {
    int saveErrno = errno;
    if (saveErrno != EAGAIN && saveErrno != EWOULDBLOCK && saveErrno != EINTR) {
    }
  }
  return connfd;
}

int Socket::createNonblocking() {
  int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  assert(fd >= 0);
  return fd;
}
