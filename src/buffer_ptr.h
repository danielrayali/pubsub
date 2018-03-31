#pragma once

#include "types.h"

namespace pubsub {

struct BufferPtr {
  Byte* ptr = nullptr;

  size_t size = 0;

  BufferPtr(Byte* _ptr, const size_t _size) :
    ptr(_ptr), size(_size) {}
};

}  // namespace pubsub
