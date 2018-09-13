#include "messages.h"
#include <memory>
#include <string>
#include <cstring>

using namespace std;

namespace pubsub {

string ToString(const MessageType& message_type) {
  return to_string(static_cast<int32_t>(message_type));
}

}  // namespace pubsub

