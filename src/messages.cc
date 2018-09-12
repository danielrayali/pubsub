#include "messages.h"

namespace pubsub {

std::string ToString(const MessageType& message_type) {
  return std::to_string(static_cast<int32_t>(message_type));
}

}  // namespace pubsub

