
#pragma once

#include <sys/epoll.h>
#include <vector>

namespace reactor::net {
class EventLoop;
class Channel;

class Poller {
public:
  using ChannelList = std::vector<Channel *>;

  explicit Poller(EventLoop *loop) : ownerLoop_(loop) {}
  virtual ~Poller() = default;

  Poller(Poller const &) = delete;
  Poller &operator=(Poller const &) = delete;

  virtual void poll(int timeoutMs, ChannelList *activeChannels) = 0;

  virtual void updateChannel(Channel *) = 0;
  virtual void removeChannel(Channel *) = 0;

protected:
  EventLoop *ownerLoop_;
};

} // namespace reactor::net
