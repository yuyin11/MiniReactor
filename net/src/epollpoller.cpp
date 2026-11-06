
#include "reactor/net/epollpoller.h"
#include "reactor/net/channel.h"
#include "reactor/net/poller.h"

#include <cassert>
#include <cerrno>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

namespace reactor::net {
EpollPoller::EpollPoller(EventLoop *loop)
    : Poller(loop), epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize) {
  assert(epollfd_ >= 0);
}

EpollPoller::~EpollPoller() { ::close(epollfd_); }

void EpollPoller::poll(int timeoutMs, ChannelList *activeChannels) {
  int numEvents = ::epoll_wait(epollfd_, events_.data(),
                               static_cast<int>(events_.size()), timeoutMs);
  int savedErrno = errno;

  if (numEvents > 0) {
    fillActiveChannels(numEvents, activeChannels);
    // resize
    if (static_cast<size_t>(numEvents) == events_.size()) {
      events_.resize(events_.size() * 2);
    }
  } else if (numEvents < 0) {
    if (savedErrno != EINTR) {
      // logger
    }
  }
}

void EpollPoller::fillActiveChannels(int numEvents,
                                     ChannelList *activeChannels) const {
  for (int i = 0; i < numEvents; ++i) {
    auto *channel = static_cast<Channel *>(events_[i].data.ptr);
    channel->setRevents(events_[i].events);
    activeChannels->push_back(channel);
  }
}

void EpollPoller::updateChannel(Channel *channel) {
  int index = channel->index();
  int fd = channel->fd();

  if (index == Channel::kNew || index == Channel::kDeleted) {
    if (index == Channel::kNew) {
      channels_[fd] = channel;
    }
    update(EPOLL_CTL_ADD, channel);
  }
}

void EpollPoller::removeChannel(Channel *channel) {
  int fd = channel->fd();
  channels_.erase(fd);

  if (channel->index() == Channel::kAdded) {
    update(EPOLL_CTL_DEL, channel);
  }
  channel->setIndex(Channel::kNew);
}

void EpollPoller::update(int operation, Channel *channel) {
  epoll_event ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.events = channel->event();
  ev.data.ptr = channel;

  if (::epoll_ctl(epollfd_, operation, channel->fd(), &ev)) {
    // logger;
  }
}

} // namespace reactor::net
