#include "buffer.h"
#include <cstddef>
#include <cstring>
#include <utility>
#include "buffer_ptr.h"
#include "types.h"

namespace pubsub {

Buffer::Buffer(const size_t bytes) {
  this->Allocate(bytes);
}

Buffer::Buffer(Buffer&& buffer) {
  if (&buffer == this)
    return;

  std::swap(buffer_, buffer.buffer_);
  std::swap(size_, buffer.size_);
  std::swap(allocated_, buffer.allocated_);
}

Buffer& Buffer::operator=(Buffer&& buffer) {
  if (&buffer == this)
    return *this;

  std::swap(buffer_, buffer.buffer_);
  std::swap(size_, buffer.size_);
  std::swap(allocated_, buffer.allocated_);

  return *this;
}

Buffer::Buffer(const Buffer& buffer) {
  if (&buffer == this)
    return;

  this->Allocate(buffer.size_);
  memcpy(buffer_.get(), buffer.buffer_.get(), buffer.size_);
}

Buffer& Buffer::operator=(const Buffer& buffer) {
  if (&buffer == this)
    return *this;

  this->Allocate(buffer.size_);
  memcpy(buffer_.get(), buffer.buffer_.get(), buffer.size_);
  return *this;
}

Buffer::~Buffer() {
  this->Free();
}

void Buffer::Allocate(const size_t bytes) {
  if (allocated_ >= bytes) {
    size_ = bytes;
    return;
  }

  if (buffer_ != nullptr)
    buffer_.reset();

  buffer_.reset(new Byte[bytes]);
  size_ = bytes;
  allocated_ = bytes;
}

void Buffer::Allocate(const size_t bytes, const Byte init) {
  this->Allocate(bytes);
  for (size_t i = 0; i < size_; ++i)
      buffer_.get()[i] = init;
}

void Buffer::Free() {
  if (buffer_ != nullptr)
    buffer_.reset();

  buffer_ = nullptr;
  size_ = 0;
  allocated_ = 0;
}

size_t Buffer::Size() const {
  return size_;
}

Byte* Buffer::Get() {
  return buffer_.get();
}

const Byte* const Buffer::Get() const {
  return buffer_.get();
}

Buffer::operator BufferPtr() const {
  return BufferPtr(buffer_.get(), size_);
}

Buffer::operator bool() const {
  return (buffer_ != nullptr);
}

}  // namespace pubsub
