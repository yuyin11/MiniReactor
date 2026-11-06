
#pragma once

#include <cstddef>
#include <string>
#include <sys/types.h>
#include <vector>

namespace reactor {
class Buffer {
public:
  static int const kCheapPrepend = 8;
  static int const kInitialSize = 1024;

  explicit Buffer(size_t initialSize = kInitialSize);

  size_t readableBytes() const { return writerIndex_ - readerIndex_; }
  size_t writableBytes() const { return buffer_.size() - writerIndex_; }
  size_t prependableBytes() const { return readerIndex_; }

  char const *peek() { return begin() + readerIndex_; }

  void retrieve(size_t len);
  void retrieveAll();
  std::string retrieveAsString(size_t len);
  std::string retrieveAllAsString();

  void append(char const *data, size_t len);
  void append(std::string const &str);

  ssize_t readFd(int fd, int *savedErrno);

private:
  char *begin() { return buffer_.data(); }
  char const *begin() const { return buffer_.data(); }
  char *beginWrite() { return begin() + writerIndex_; }
  char const *beginWrite() const { return begin() + writerIndex_; }

  void ensureWritalbeBytes(size_t len);
  void makeSpace(size_t len);

  std::vector<char> buffer_;
  size_t readerIndex_;
  size_t writerIndex_;
};

} // namespace reactor
