
#pragma once

#include "poller.h"

#include <memory>
#include <sys/epoll.h>
#include <vector>

namespace reactor::net {
class Channel;

class EventLoop {
public:
  using ChannelList = std::vector<Channel *>;

  EventLoop();
  ~EventLoop();

  EventLoop(EventLoop const &) = delete;
  EventLoop operator=(EventLoop const &) = delete;

  void loop();
  void quit() { quit_ = true; }

  void updateChannel(Channel *);
  void removeChannel(Channel *);

private:
  std::unique_ptr<Poller> poller_;
  ChannelList activeChannels_;
  bool quit_{false};

  // int epollfd_;
  // std::vector<epoll_event> events_;
  // std::map<int, Channel *> channels_;
};

} // namespace reactor::net
