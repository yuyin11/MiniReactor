
#pragma once

#include "eventloop.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <sys/epoll.h>

class Channel {
public:
  Channel(EventLoop *loop, int fd);
  ~Channel();

  void setReadCallback(std::function<void()> cb) {
    readCallback_ = std::move(cb);
  }
  void setWriteCallback(std::function<void()> cb) {
    writeCallback_ = std::move(cb);
  }
  void setCloseCallback(std::function<void()> cb) {
    closeCallback_ = std::move(cb);
  }
  void setErrorCallback(std::function<void()> cb) {
    errorCallback_ = std::move(cb);
  }

  void enableReading();
  void disableReading();
  void enableWriting();
  void disableWriting();
  void disableAll();
  void remove();

  bool isNoneEvents() { return events_ == 0; }
  bool isReading() { return events_ & (EPOLLIN | EPOLLPRI); }
  bool isWriting() { return events_ & EPOLLOUT; }

  int fd() const { return fd_; }
  uint32_t event() const { return events_; }
  void setRevents(uint32_t revt) { revents_ = revt; }
  int index() const { return index_; }
  void setIndex(int idx) { index_ = idx; }

  static int const kNew = -1;
  static int const kAdded = 1;
  static int const kDeleted = 2;

  void tie(std::shared_ptr<void> const &obj);

  void handleEvent();

private:
  void update();
  void handleEventWithGuard();

  EventLoop *loop_;
  int fd_;
  uint32_t events_{0};
  uint32_t revents_{0};
  int index_{-1};

  std::weak_ptr<void> tie_;
  bool tied_{false};

  std::function<void()> readCallback_;
  std::function<void()> writeCallback_;
  std::function<void()> closeCallback_;
  std::function<void()> errorCallback_;
};
