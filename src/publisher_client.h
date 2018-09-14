#pragma once

#include <string>

namespace pubsub {

class PublisherClient {
 public:
  PublisherClient(const std::string& host, const uint16_t port);

  ~PublisherClient() = default;

  void Publish(const void* const data, const uint64_t size) const;

 private:
  std::string host_;
  uint16_t port_;
};

}  // namespace pubsub
