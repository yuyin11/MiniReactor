
#include "../include/reactor/net/inetaddress.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

InetAddress::InetAddress(uint16_t port, bool loopbackOnly) {
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  addr_.sin_addr.s_addr =
      loopbackOnly ? htonl(INADDR_LOOPBACK) : htonl(INADDR_ANY);
}

uint16_t InetAddress::port() const { return ntohs(addr_.sin_port); }

std::string InetAddress::toIp() const {
  char buf[INET_ADDRSTRLEN] = {0};
  ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
  return buf;
}

std::string InetAddress::toIpPort() const {
  return toIp() + ":" + std::to_string(port());
}
