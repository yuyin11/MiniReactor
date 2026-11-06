
#include "reactor/net/eventloop.h"
#include "reactor/net/inetaddress.h"
#include "reactor/net/tcpserver.h"
#include <cstdio>
#include <string>

using namespace reactor;

int main() {
  EventLoop loop;
  InetAddress listenAddr(8888);
  TcpServer server(&loop, listenAddr);

  server.setMessageCallback([](TcpConnection::TcpConnectionPtr const &conn,
                               Buffer *buf) {
    std::string msg = buf->retrieveAllAsString();
    fprintf(stderr, "[echo] fd=%d recv %zu bytes\n", conn->fd(), msg.size());
    conn->send(msg);
  });

  server.start();
  loop.loop();
  return 0;
}
