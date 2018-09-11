#pragma once

#include <cstddef>
#include <memory>
#include "buffer_ptr.h"
#include "types.h"

namespace pubsub {

class Buffer {
 public:
  Buffer() = default;

  Buffer(const size_t bytes);

  Buffer(Buffer&& buffer);

  Buffer& operator=(Buffer&& buffer);

  Buffer(const Buffer& buffer);

  Buffer& operator=(const Buffer& buffer);

  ~Buffer();

  void Allocate(const size_t bytes);

  void Allocate(const size_t bytes, const Byte init);

  void Free();

  size_t Size() const;

  Byte* Get();

  const Byte* const Get() const;

  operator BufferPtr() const;

  operator bool() const;

 private:
  std::unique_ptr<Byte> buffer_;
  size_t size_ = 0;
  size_t allocated_ = 0;
};

}  // namespace pubsub
