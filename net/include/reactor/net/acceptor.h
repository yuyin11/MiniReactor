
#pragma once

#include "channel.h"
#include "eventloop.h"
#include "inetaddress.h"
#include "socket.h"

#include <functional>

namespace reactor::net {
class Acceptor {
public:
  using NewConnectionCallback =
      std::function<void(int connfd, InetAddress const &)>;

  Acceptor(EventLoop *loop, InetAddress const &listenAddr,
           bool reusePort = false);
  ~Acceptor();

  void setNewConnectionCallback(NewConnectionCallback cb) {
    newConnectionCallback_ = std::move(cb);
  }
  InetAddress const &listenAddr() const { return listenAddr_; }
  void listen();

private:
  void handleRead();

  EventLoop *loop_;
  Socket acceptSocket_;
  Channel acceptChannel_;
  InetAddress listenAddr_;
  bool listening_{false};

  NewConnectionCallback newConnectionCallback_;
};

} // namespace reactor::net
