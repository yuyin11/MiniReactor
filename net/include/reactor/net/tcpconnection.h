
#pragma once

#include "buffer.h"
#include "channel.h"
#include "eventloop.h"
#include "inetaddress.h"
#include "socket.h"

#include <functional>
#include <memory>
#include <string>

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  using MessageCallback =
      std::function<void(TcpConnectionPtr const &, Buffer *)>;
  using CloseCallback = std::function<void(TcpConnectionPtr const &)>;
  using WriteCompleteCallback = std::function<void(TcpConnectionPtr const &)>;

  TcpConnection(EventLoop *loop, int connfd, InetAddress const &peerAddr);
  ~TcpConnection();

  void start();
  void send(std::string const &msg);
  void shutdown();

  void setMessageCallback(MessageCallback cb) {
    messageCallback_ = std::move(cb);
  }
  void setCloseCallback(CloseCallback cb) { closeCallback_ = std::move(cb); }
  void setWriteCompleteCallback(WriteCompleteCallback cb) {
    writeCompleteCallback_ = std::move(cb);
  }

  int fd() const { return socket_.fd(); }
  InetAddress const &peerAddr() const { return peerAddr_; }
  bool connected() const { return state_ == kConnected; }

private:
  void handleRead();
  void handleWrite();
  void handleClose();
  void handleError();
  void sendInLoop(std::string const &msg);

  enum StateE { kConnecting, kConnected, kDisconnecting, kDisconnected };

  Buffer inputBuffer_;
  Buffer outputBuffer_;

  EventLoop *loop_;
  Socket socket_;
  Channel channel_;
  InetAddress peerAddr_;

  StateE state_{kConnecting};

  MessageCallback messageCallback_;
  CloseCallback closeCallback_;
  WriteCompleteCallback writeCompleteCallback_;
};
