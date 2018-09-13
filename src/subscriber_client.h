#pragma once

#include <future>
#include <string>
#include "asio.h"
#include "messages.h"

namespace pubsub {

class SubscriberClient {
 public:
  SubscriberClient(const std::string& host, const uint16_t port);

  ~SubscriberClient() = default;

  int32_t Register();

  void Deregister(const int32_t client_id);

 private:
  void DoRead();

  std::string host_;
  uint16_t port_;
  asio::ip::tcp::socket socket_;
  std::future<void> result_;
  MessageType message_type_;
};

}  // namespace pubsub
