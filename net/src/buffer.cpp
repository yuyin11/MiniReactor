
#include "reactor/net/buffer.h"
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/uio.h>

using namespace reactor;

Buffer::Buffer(size_t initialSize)
    : buffer_(initialSize), readerIndex_(kCheapPrepend),
      writerIndex_(kCheapPrepend) {}

void Buffer::retrieve(size_t len) {
  if (len < readableBytes()) {
    readerIndex_ += len;
  } else {
    retrieveAll();
  }
}

void Buffer::retrieveAll() {
  readerIndex_ = kCheapPrepend;
  writerIndex_ = kCheapPrepend;
}

std::string Buffer::retrieveAsString(size_t len) {
  std::string result(peek(), len);
  retrieve(len);
  return result;
}

std::string Buffer::retrieveAllAsString() {
  return retrieveAsString(readableBytes());
}

void Buffer::append(char const *data, size_t len) {
  ensureWritalbeBytes(len);
  std::memcpy(beginWrite(), data, len);
  writerIndex_ += len;
}

void Buffer::append(std::string const &str) { append(str.data(), str.size()); }

ssize_t Buffer::readFd(int fd, int *savedErrno) {
  char extrabuf[65536];
  iovec vec[2];

  size_t const writable = writableBytes();
  vec[0].iov_base = beginWrite();
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof(extrabuf);

  int const iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
  ssize_t n = ::readv(fd, vec, iovcnt);

  if (n < 0) {
    *savedErrno = errno;
  } else if (static_cast<size_t>(n) <= writable) {
    writerIndex_ += n;
  } else {
    writerIndex_ = buffer_.size();
    append(extrabuf, n - writable);
  }
  return n;
}

void Buffer::ensureWritalbeBytes(size_t len) {
  if (writableBytes() < len) {
    makeSpace(len);
  }
}

void Buffer::makeSpace(size_t len) {
  if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
    buffer_.resize(writerIndex_ + len);
  } else {
    size_t readable = readableBytes();
    std::memmove(begin() + kCheapPrepend, begin() + readerIndex_, readable);
    readerIndex_ = kCheapPrepend;
    writerIndex_ = readerIndex_ + readable;
  }
}
