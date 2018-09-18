#include <gtest/gtest.h>
#include <fstream>
#include "config.h"

using namespace pubsub;
using namespace std;

TEST(ConfigTest, TopicToFromString) {
  TopicConfig topic_config;
  topic_config.name = "topic1";
  topic_config.port = 12346;

  string buffer = topic_config.ToString();
  TopicConfig read_config;
  read_config.FromString(buffer);

  ASSERT_STREQ(topic_config.name.c_str(), read_config.name.c_str());
  ASSERT_EQ(topic_config.port, read_config.port);
}

TEST(ConfigTest, ToFromString) {
  Config write_config;
  write_config.master_port = 12345;

  for (int i = 0; i < 10; ++i) {
    TopicConfig topic_config;
    topic_config.name = "topic" + to_string(i);
    topic_config.port = 12346 + i;
    write_config.topic_configs.push_back(topic_config);
  }

  string buffer = write_config.ToString();

  Config read_config;
  read_config.FromString(buffer);

  ASSERT_EQ(read_config.master_port, write_config.master_port);
  for (int i = 0; i < 10; ++i) {
    ASSERT_STREQ(read_config.topic_configs[i].name.c_str(), write_config.topic_configs[i].name.c_str());
    ASSERT_EQ(read_config.topic_configs[i].port, write_config.topic_configs[i].port);
  }
}
