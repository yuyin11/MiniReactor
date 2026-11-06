
#include "reactor/net/tcpconnection.h"
#include "reactor/net/eventloop.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

namespace reactor::net {
TcpConnection::TcpConnection(EventLoop *loop, int connfd,
                             InetAddress const &peerAddr)
    : loop_(loop), socket_(connfd), channel_(loop, connfd), peerAddr_(peerAddr),
      state_(kConnected) {
  channel_.setReadCallback([this] { handleRead(); });
  channel_.setWriteCallback([this] { handleWrite(); });
  channel_.setCloseCallback([this] { handleClose(); });
  channel_.setErrorCallback([this] { handleError(); });
}

TcpConnection::~TcpConnection() {
  fprintf(stderr, "[TcpConnection] fd=%d destroyed\n", socket_.fd());
}

void TcpConnection::start() {
  state_ = kConnected;
  channel_.tie(shared_from_this());
  channel_.enableReading();
}

void TcpConnection::handleRead() {
  int savedErrno = 0;
  ssize_t n = inputBuffer_.readFd(socket_.fd(), &savedErrno);

  if (n > 0) {
    if (messageCallback_) {
      messageCallback_(shared_from_this(), &inputBuffer_);
    }
  } else if (n == 0) {
    handleClose();
  } else {
    errno = savedErrno;
    handleError();
  }
}

void TcpConnection::send(std::string const &msg) {
  if (state_ != kConnected) {
    return;
  }
  sendInLoop(msg);
}

void TcpConnection::sendInLoop(const std::string &msg) {
  ssize_t nwrote = 0;
  size_t remaining = msg.size();
  bool faultError = false;

  if (!channel_.isWriting() && outputBuffer_.readableBytes() == 0) {
    nwrote = ::write(socket_.fd(), msg.data(), msg.size());
    if (nwrote < 0) {
      nwrote = 0;
      if (errno != EWOULDBLOCK && errno != EAGAIN) {
        faultError = true;
      }
    }
    remaining = msg.size() - nwrote;
  }

  if (remaining > 0 && !faultError) {
    outputBuffer_.append(msg.data() + nwrote, remaining);
    if (!channel_.isWriting()) {
      channel_.enableWriting();
    }
  }

  if (faultError) {
    handleError();
  }
}

void TcpConnection::handleWrite() {
  if (state_ == kDisconnected) {
    return;
  }
  if (!channel_.isWriting()) {
    return;
  }

  ssize_t n = ::write(socket_.fd(), outputBuffer_.peek(),
                      outputBuffer_.readableBytes());

  if (n > 0) {
    outputBuffer_.retrieve(n);
    if (outputBuffer_.readableBytes() == 0) {
      channel_.disableWriting();
      if (writeCompleteCallback_) {
        writeCompleteCallback_(shared_from_this());
      }
      if (state_ == kDisconnecting) {
        shutdown();
      }
    }
  } else {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      handleError();
    }
  }
}

void TcpConnection::shutdown() {
  if (state_ == kConnected) {
    state_ = kDisconnecting;
    if (!channel_.isWriting()) {
      ::shutdown(socket_.fd(), SHUT_WR);
    }
  }
}

void TcpConnection::handleClose() {
  if (state_ == kDisconnected) {
    return;
  }
  state_ = kDisconnected;

  channel_.disableAll();
  channel_.remove();

  if (closeCallback_) {
    closeCallback_(shared_from_this());
  }
}

void TcpConnection::handleError() {
  fprintf(stderr, "[TcpConnection] fd=%d error: %s\n", socket_.fd(),
          strerror(errno));
  handleClose();
}

} // namespace reactor::net
