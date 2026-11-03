
#include "../include/reactor/net/channel.h"
#include <memory>
#include <sys/epoll.h>

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd) {}

Channel::~Channel() = default;

void Channel::enableReading() {
  events_ |= (EPOLLIN | EPOLLPRI | EPOLLRDHUP);
  update();
}

void Channel::enableWriting() {
  events_ |= EPOLLOUT;
  update();
}

void Channel::disableReading() {
  events_ &= ~(EPOLLIN | EPOLLPRI | EPOLLRDHUP);
  update();
}

void Channel::disableWriting() {
  events_ &= ~EPOLLOUT;
  update();
}

void Channel::disableAll() {
  events_ = 0;
  update();
}

void Channel::remove() { loop_->removeChannel(this); }

void Channel::tie(std::shared_ptr<void> const &obj) {
  tie_ = obj;
  tied_ = true;
}

void Channel::handleEvent() {
  if (tied_) {
    std::shared_ptr<void> guard = tie_.lock();
    if (guard) {
      handleEventWithGuard();
    }
  } else {
    handleEventWithGuard();
  }
}

void Channel::update() { loop_->updateChannel(this); }

void Channel::handleEventWithGuard() {
  if (revents_ & (EPOLLERR | EPOLLHUP)) {
    if (errorCallback_)
      errorCallback_();
  }
  if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
    if (readCallback_)
      readCallback_();
  }
  if (revents_ & EPOLLOUT) {
    if (writeCallback_)
      writeCallback_();
  }
}
