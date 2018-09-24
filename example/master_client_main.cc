#include <pubsub.h>
#include <sargs.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace pubsub;

int main(int argc, char* argv[]) {
  try {
    SARGS_REQUIRED_FLAG_VALUE("--mode", "-m", "Must be either \"query\" or \"add\"");
    SARGS_REQUIRED_FLAG_VALUE("--master-port", "-mp", "The master port to connect to");
    SARGS_OPTIONAL_FLAG_VALUE("--port", "-p", "The port for the new topic");
    SARGS_OPTIONAL_FLAG_VALUE("--name", "-n", "The name for the new topic");
    SARGS_INITIALIZE(argc, argv);

    uint16_t master_port = SARGS_GET_UINT16("-mp");
    MasterClient master_client("localhost", master_port);
    if (SARGS_GET_STRING("--mode") == "query") {
      vector<TopicConfig> topic_configs = master_client.QueryForTopics();
      for (auto topic_config : topic_configs)
        cout << "Topic: name=" << topic_config.name << ", port=" << topic_config.port << endl;
    } else if (SARGS_GET_STRING("--mode") == "add") {
      TopicConfig topic_config;
      topic_config.port = SARGS_GET_UINT16("--port");
      topic_config.name = SARGS_GET_STRING("--name");
      master_client.AddTopic(topic_config);
    }

    return 0;
  } catch (exception& ex) {
    cerr << "Error caught in main: " << ex.what() << endl;
  }
}