#pragma once

#include <future>
#include <functional>
#include <string>
#include "asio.h"
#include "buffer.h"
#include "messages.h"

namespace pubsub {

class SubscriberClient {
 public:
  SubscriberClient(const std::string& host, const uint16_t port);

  ~SubscriberClient();

  void Register(const std::function<void(pubsub::Buffer&& buffer)>& callback);

  void Deregister();

 private:
  void DoRead();

  std::string host_;
  uint16_t port_;
  asio::ip::tcp::socket socket_;
  std::future<void> result_;
  MessageType message_type_;
  std::function<void(pubsub::Buffer&& buffer)> callback_;
  int32_t token_;
};

}  // namespace pubsub
