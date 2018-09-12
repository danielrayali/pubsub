#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

namespace pubsub {

typedef uint8_t Byte;

typedef uint64_t ByteSize;

inline size_t ToSizeT(ByteSize byte_size) {
  if (byte_size > std::numeric_limits<size_t>::max())
    throw std::range_error("Cannot convert " + std::to_string(byte_size) + " to size_t");
  return static_cast<size_t>(byte_size);
}

}  // namespace pubsub
