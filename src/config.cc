#include "config.h"
#include <fstream>
#include <iostream>

using namespace std;

namespace pubsub {

Config::Config(const std::string& path) {
  this->Parse(path);
}

static vector<string> Split(const char delim, const string& input) {
  vector<string> tokens;
  size_t last = 0;
  size_t cur = input.find_first_of(delim);
  while (cur != string::npos) {
    tokens.push_back(input.substr(last, cur - last));
    last = cur + 1;
    cur = input.find_first_of(delim, last);
  }

  if (last != input.size())
    tokens.push_back(input.substr(last));

  return tokens;  
}

void Config::WriteToFile(const std::string& path) const {
  ofstream output(path);
  output << "master,port=" << master_config_.port << endl;
  for (const TopicConfig& topic_config : topic_configs_) {
    output << "topic,port=" << topic_config.port
           << ",name=" << topic_config.name << endl;
  }
}

void Config::Parse(const std::string& path) {
  ifstream input(path);
  string line;
  while(input) {
    getline(input, line);
    size_t pos = line.find_first_not_of(" \t");
    if (pos == string::npos || line[pos] == '#')
      continue;
    
	  vector<string> line_tokens = Split(',', line);
    if (line_tokens[0] == "master") {
      for (size_t i = 1; i < line_tokens.size(); ++i) {
        vector<string> kv_pair = Split('=', line_tokens[i]);
        if (kv_pair[0] == "port")
          master_config_.port = atoi(kv_pair[1].c_str());
      }      
    } else if (line_tokens[0] == "topic") {
      TopicConfig topic_config;
      for (size_t i = 1; i < line_tokens.size(); ++i) {
        vector<string> kv_pair = Split('=', line_tokens[i]);
        if (kv_pair[0] == "port")
          topic_config.port = atoi(kv_pair[1].c_str());
        else if (kv_pair[0] == "name")
          topic_config.name = kv_pair[1];
      }
      topic_configs_.push_back(topic_config);
    }
  }
}

void Config::SetMasterConfig(const MasterConfig& master_config) {
  master_config_ = master_config;
}

MasterConfig Config::GetMasterConfig() const {
  return master_config_;
}

void Config::SetTopicConfigs(const std::vector<TopicConfig>& topic_configs) {
  topic_configs_ = topic_configs;
}

vector<TopicConfig> Config::GetTopicConfigs() const {
  return topic_configs_;
}

}  // namespace pubsub
