// Acceptor.cpp
#include "reactor/net/acceptor.h"
#include "reactor/net/eventloop.h"
#include <unistd.h>

namespace reactor::net {
Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr,
                   bool reusePort)
    : loop_(loop), acceptSocket_(Socket::createNonblocking()),
      acceptChannel_(loop, acceptSocket_.fd()), listenAddr_(listenAddr) {

  acceptSocket_.setReuseAddr(true);
  acceptSocket_.setReusePort(reusePort);
  acceptSocket_.bind(*listenAddr.getSockAddr());

  acceptChannel_.setReadCallback([this] { handleRead(); });
}

Acceptor::~Acceptor() {
  acceptChannel_.disableAll();
  acceptChannel_.remove();
}

void Acceptor::listen() {
  acceptSocket_.listen();
  listening_ = true;
  acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
  struct sockaddr_in peerAddr;
  int connfd = acceptSocket_.accept(&peerAddr);

  if (connfd >= 0) {
    InetAddress peer(peerAddr);
    if (newConnectionCallback_) {
      newConnectionCallback_(connfd, peer);
    } else {
      ::close(connfd);
    }
  }
}

} // namespace reactor::net
