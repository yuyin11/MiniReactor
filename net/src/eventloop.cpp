
#include "../include/reactor/net/eventloop.h"
#include "../include/reactor/net/channel.h"

#include <cerrno>
#include <cstdio>
#include <sys/epoll.h>
#include <unistd.h>

EventLoop::EventLoop() {
  epollfd_ = epoll_create1(EPOLL_CLOEXEC);
  events_.resize(16);
}

EventLoop::~EventLoop() { ::close(epollfd_); }

void EventLoop::loop() {
  while (!quit_) {
    int n = epoll_wait(epollfd_, events_.data(), (int)events_.size(), 10000);
    if (n < 0) {
      // Interrupted system call
      if (errno == EINTR)
        continue;
      perror("epoll_wait");
      break;
    }

    for (int i = 0; i < n; ++i) {
      Channel *ch = static_cast<Channel *>(events_[i].data.ptr);
      ch->setRevents(events_[i].events);
      ch->handleEvent();
    }

    if (n == (int)events_.size()) {
      events_.resize(events_.size() * 2);
    }
  }
}

void EventLoop::updateChannel(Channel *channel) {
  int fd = channel->fd();
  epoll_event ev{};
  ev.events = channel->event();
  ev.data.ptr = channel;

  if (channel->index() == Channel::kNew) {
    channels_[fd] = channel;
    ::epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev);
    channel->setIndex(Channel::kAdded);
  } else if (channel->index() == Channel::kAdded) {
    ::epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev);
  }
}

void EventLoop::removeChannel(Channel *channel) {
  int fd = channel->fd();
  channels_.erase(fd);
  ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr);
  channel->setIndex(Channel::kNew);
}
