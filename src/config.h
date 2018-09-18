#pragma once

#include <string>
#include <vector>

namespace pubsub {

struct TopicConfig {
  std::string name;

  uint16_t port;

  std::string ToString() const;

  void FromString(const std::string& input);
};

struct Config {
  uint16_t master_port;

  std::vector<TopicConfig> topic_configs;

  std::string ToString() const;

  void FromString(const std::string& input);
};

}  // namespace pubsub
