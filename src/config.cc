#include "config.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace pubsub {

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

static void ParseLine(const string& line, Config& config) {
  size_t pos = line.find_first_not_of(" \t");
  if (pos == string::npos || line[pos] == '#')
    return;

  vector<string> line_tokens = Split(',', line);
  if (line_tokens[0] == "master") {
    for (size_t i = 1; i < line_tokens.size(); ++i) {
      vector<string> kv_pair = Split('=', line_tokens[i]);
      if (kv_pair[0] == "port")
        config.master_port = atoi(kv_pair[1].c_str());
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
    config.topic_configs.push_back(topic_config);
  }
}

string TopicConfig::ToString() const {
  stringstream temp;
  temp << static_cast<uint64_t>(this->name.size()) << this->name << this->port;
  return temp.str();
}

void TopicConfig::FromString(const std::string& input) {
  stringstream temp(input);
  uint64_t size = 0;
  temp >> size;
  this->name.resize(static_cast<unsigned>(size));
  temp.read(&this->name.front(), size);
  temp >> this->port;
}

string Config::ToString() const {
  stringstream temp;
  temp << "master,port=" << this->master_port << endl;
  for (const TopicConfig& topic_config : this->topic_configs) {
    temp << "topic,port=" << topic_config.port
           << ",name=" << topic_config.name << endl;
  }
  return temp.str();
}

void Config::FromString(const string& input) {
  vector<string> lines = Split('\n', input);
  for (const string& line : lines) {
    ParseLine(line, *this);
  }
}

}  // namespace pubsub
