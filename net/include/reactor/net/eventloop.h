
#pragma once

#include <map>
#include <sys/epoll.h>
#include <vector>

namespace reactor {
class Channel;

class EventLoop {
public:
  EventLoop();
  ~EventLoop();

  void loop();
  void quit() { quit_ = true; }

  void updateChannel(Channel *);
  void removeChannel(Channel *);

private:
  int epollfd_;
  bool quit_{false};

  std::vector<epoll_event> events_;
  std::map<int, Channel *> channels_;
};

} // namespace reactor
