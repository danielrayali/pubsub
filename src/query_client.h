#pragma once

#include "asio.h"

namespace pubsub {

class QueryClient {
 public:
  QueryClient(const std::string& host, const uint16_t port);

  ~QueryClient() = default;

  std::vector<std::string> QueryForTopics();

 private:
  std::string host_;
  uint16_t port_;
};

}  // namespace pubsub
