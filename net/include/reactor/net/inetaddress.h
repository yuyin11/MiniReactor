
#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <string>

namespace reactor::net {
class InetAddress {
public:
  // listenAddr
  InetAddress(uint16_t port, bool loopbackOnly = false);
  // peerAddr
  InetAddress(sockaddr_in const &addr) : addr_(addr) {}

  // bind
  sockaddr_in const *getSockAddr() const { return &addr_; }
  void setSocketAddr(sockaddr_in const &addr) { addr_ = addr; }

  uint16_t port() const;
  std::string toIp() const;
  std::string toIpPort() const;

private:
  sockaddr_in addr_;
};

} // namespace reactor::net
