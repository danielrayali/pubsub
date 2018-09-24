#pragma once

#include <vector>
#include <string>
#include "asio.h"
#include "config.h"

namespace pubsub {

// Will manage topics for a MasterServer. You can query, add, and
// remove topics from the MasterServer.
class MasterClient {
 public:
  MasterClient(const std::string& host, const uint16_t port);

  ~MasterClient() = default;

  std::vector<TopicConfig> QueryForTopics();

  void AddTopic(const TopicConfig& topic_config);

  void RemoveTopic(const std::string& topic_id);

 private:
  std::string host_;
  uint16_t port_;
};

}  // namespace pubsub
