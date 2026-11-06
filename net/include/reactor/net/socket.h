
#pragma once

#include <netinet/in.h>

namespace reactor {
class Socket {
public:
  Socket(int fd) : fd_(fd) {}
  ~Socket();

  Socket(Socket const &) = delete;
  Socket operator=(Socket const &) = delete;

  int fd() const { return fd_; }

  void setNonBlocking(bool on);
  void setReusePort(bool on);
  void setReuseAddr(bool on);

  void bind(sockaddr_in const &addr);
  void listen();
  int accept(sockaddr_in *peerAddr);

  static int createNonblocking();

private:
  int fd_;
};

} // namespace reactor
