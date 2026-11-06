
#include "reactor/net/eventloop.h"
#include "reactor/net/channel.h"
#include "reactor/net/epollpoller.h"
#include "reactor/net/poller.h"

#include <memory>
#include <sys/epoll.h>
#include <unistd.h>

namespace reactor::net {
EventLoop::EventLoop() : poller_(std::make_unique<EpollPoller>(this)) {}

EventLoop::~EventLoop() = default;

void EventLoop::loop() {
  while (!quit_) {
    activeChannels_.clear();
    poller_->poll(10000, &activeChannels_);

    for (Channel *ch : activeChannels_) {
      ch->handleEvent();
    }
  }
}

void EventLoop::updateChannel(Channel *channel) {
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
  poller_->removeChannel(channel);
}

} // namespace reactor::net
