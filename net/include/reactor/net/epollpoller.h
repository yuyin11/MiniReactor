
#pragma once

#include "reactor/net/poller.h"
#include <map>

namespace reactor::net {

class EpollPoller : public Poller {
public:
  explicit EpollPoller(EventLoop *loop);
  ~EpollPoller() override;

  void poll(int timeoutMs, ChannelList *activeChannels) override;
  void updateChannel(Channel *channel) override;
  void removeChannel(Channel *channel) override;

private:
  static int const kInitEventListSize = 16;

  void update(int operation, Channel *channel);
  void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;

  int epollfd_;
  std::vector<epoll_event> events_;
  std::map<int, Channel *> channels_;
};
} // namespace reactor::net
