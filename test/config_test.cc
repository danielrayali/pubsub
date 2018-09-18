#include <gtest/gtest.h>
#include <fstream>
#include "config.h"

using namespace pubsub;
using namespace std;

TEST(ConfigTest, BasicParseTest) {
  ofstream out_temp("temp.config");
  out_temp << "master,port=10000\n"
       << "topic,name=topic1,port=10001\n"
       << "topic,name=othertopic,port=10002" << endl;
  out_temp.close();

  Config config("temp.config");
  MasterConfig master_config = config.GetMasterConfig();
  ASSERT_EQ(master_config.port, 10000);

  vector<TopicConfig> topic_configs = config.GetTopicConfigs();
  ASSERT_EQ(topic_configs.size(), 2);
  ASSERT_STREQ(topic_configs[0].name.c_str(), "topic1");
  ASSERT_EQ(topic_configs[0].port, 10001);
  ASSERT_STREQ(topic_configs[1].name.c_str(), "othertopic");
  ASSERT_EQ(topic_configs[1].port, 10002);

  std::remove("temp.config");
}

TEST(ConfigTest, WriteAndParseTest) {
  MasterConfig master_config;
  master_config.port = 12345;

  vector<TopicConfig> topic_configs;
  for (int i = 0; i < 10; ++i) {
    TopicConfig topic_config;
    topic_config.name = "topic" + to_string(i);
    topic_config.port = 12346 + i;
    topic_configs.push_back(topic_config);
  }

  Config write_config;
  write_config.SetMasterConfig(master_config);
  write_config.SetTopicConfigs(topic_configs);
  write_config.WriteToFile("temp.config");

  Config read_config("temp.config");
  MasterConfig read_master_config = read_config.GetMasterConfig();
  vector<TopicConfig> read_topic_configs = read_config.GetTopicConfigs();

  ASSERT_EQ(read_master_config.port, master_config.port);
  for (int i = 0; i < 10; ++i) {
    ASSERT_STREQ(read_topic_configs[i].name.c_str(), topic_configs[i].name.c_str());
    ASSERT_EQ(read_topic_configs[i].port, topic_configs[i].port);
  }
}
