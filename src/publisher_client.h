#pragma once

#include <string>

namespace pubsub {

class PublisherClient {
 public:
  PublisherClient(const std::string& host, const uint16_t port);

  PublisherClient(const std::string& host, const uint16_t port, const uint64_t block_size);

  ~PublisherClient() = default;

  void Publish(const void* data, const uint64_t size) const;

 private:
  std::string host_;
  uint16_t port_;
  uint64_t block_size_;
};

}  // namespace pubsub
