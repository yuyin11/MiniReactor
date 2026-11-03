
#include "../include/reactor/net/tcpserver.h"
#include "../include/reactor/net/acceptor.h"
#include "../include/reactor/net/eventloop.h"
#include "../include/reactor/net/inetaddress.h"
#include "../include/reactor/net/tcpconnection.h"
#include <cstdio>
#include <memory>

TcpServer::TcpServer(EventLoop *loop, InetAddress const &listenAddr,
                     bool reusePort)
    : loop_(loop), acceptor_(loop, listenAddr, reusePort) {
  acceptor_.setNewConnectionCallback(
      [this](int connfd, InetAddress const &peer) {
        newConnection(connfd, peer);
      });
}

TcpServer::~TcpServer() {
  fprintf(stderr, "[TcpServer] destructing, alive=%zu\n", connections_.size());
  for (auto &[fd, conn] : connections_) {
    conn->shutdown();
  }
  connections_.clear();
}

void TcpServer::start() {
  if (!started_) {
    started_ = true;
    acceptor_.listen();
    fprintf(stderr, "[TcpServer] listening on %s\n",
            acceptor_.listenAddr().toIpPort().c_str());
  }
}

void TcpServer::newConnection(int connfd, const InetAddress &peerAddr) {
  fprintf(stderr, "[TcpServer] new connection fd=%d from %s\n", connfd,
          peerAddr.toIpPort().c_str());
  // conn
  auto conn = std::make_shared<TcpConnection>(loop_, connfd, peerAddr);
  connections_[connfd] = conn;

  conn->setMessageCallback(messageCallback_);
  conn->setCloseCallback(
      [this](TcpConnectionPtr const &c) { removeConnection(c); });

  conn->start();
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
  int fd = conn->fd();
  fprintf(stderr, "[TcpServer] remove connection fd=%d\n", fd);
  connections_.erase(fd);
}
