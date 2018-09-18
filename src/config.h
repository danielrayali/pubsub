#pragma once

#include <string>
#include <vector>

namespace pubsub {

struct TopicConfig {
  std::string name;
  uint16_t port;
};

struct MasterConfig {
  uint16_t port;
};

class Config {
 public:
  Config() = default;

  Config(const std::string& path);  // Will call ParseFile() with path

  ~Config() = default;

  std::string ToString() const;

  void ParseString(const std::string& input);

  void WriteToFile(const std::string& path) const;

  void ParseFile(const std::string& path);

  MasterConfig GetMasterConfig() const;

  void SetMasterConfig(const MasterConfig& master_config);

  std::vector<TopicConfig> GetTopicConfigs() const;

  void SetTopicConfigs(const std::vector<TopicConfig>& topic_configs);

 private:
  void ToStream(std::ostream& output) const;

  void ParseLine(const std::string& line);

  MasterConfig master_config_;
  std::vector<TopicConfig> topic_configs_;
};

}  // namespace pubsub
