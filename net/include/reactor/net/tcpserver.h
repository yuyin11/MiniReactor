
#pragma once

#include "acceptor.h"
#include "inetaddress.h"
#include "tcpconnection.h"

#include <map>
#include <memory>

namespace reactor {
class TcpServer {
public:
  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  using MessageCallback = TcpConnection::MessageCallback;

  TcpServer(EventLoop *loop, InetAddress const &listenAddr,
            bool reusePort = false);
  ~TcpServer();

  void setMessageCallback(MessageCallback cb) {
    messageCallback_ = std::move(cb);
  }

  void start();

  size_t connectionSize() const { return connections_.size(); }

private:
  void newConnection(int connfd, InetAddress const &peerAddr);
  void removeConnection(TcpConnectionPtr const &conn);

  EventLoop *loop_;
  Acceptor acceptor_;
  std::map<int, TcpConnectionPtr> connections_;
  bool started_{false};

  MessageCallback messageCallback_;
};

} // namespace reactor
