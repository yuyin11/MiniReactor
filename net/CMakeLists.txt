
add_library(reactor_net STATIC
  src/acceptor.cpp
  src/buffer.cpp
  src/channel.cpp
  src/eventloop.cpp
  src/inetaddress.cpp
  src/socket.cpp
  src/tcpconnection.cpp
  src/tcpserver.cpp
)

target_include_directories(reactor_net
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(reactor_net PUBLIC Threads::Threads)
