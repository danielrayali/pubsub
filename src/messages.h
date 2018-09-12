#pragma once

#include <string>

namespace pubsub {

enum class MessageType : uint8_t {
	kUnknown,
	kTopicsQuery,
    kTopicsQueryReply
};

std::string ToString(const MessageType& message_type);


}  // namespace pubsub
