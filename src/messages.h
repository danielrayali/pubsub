#pragma once

#include <string>
#include <memory>
#include "asio.h"

namespace pubsub {

enum class MessageType : uint8_t {
  kUnknown,
  kTopicsQuery,
  kTopicsQueryReply,
  kSubRegister,
  kSubRegisterReply,
  kSubDeregister,
  kSubDeregisterReply,
  kShutdown,
  kPublisher,
  kPublisherReply,
  kData,
  kError
};

std::string ToString(const MessageType& message_type);

}  // namespace pubsub
