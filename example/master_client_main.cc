#include <pubsub.h>
#include <sargs.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace pubsub;

int main(int argc, char* argv[]) {
  SARGS_REQUIRED_FLAG_VALUE("--mode", "-m", "Must be either \"query\" or \"add\"");
  SARGS_OPTIONAL_FLAG_VALUE("--port", "-p", "The port for the new topic");
  SARGS_OPTIONAL_FLAG_VALUE("--name", "-n", "The name for the new topic");
  SARGS_INITIALIZE(argc, argv);

  MasterClient master_client("localhost", 10000);
  if (SARGS_GET_STRING("--mode") == "query") {
    vector<string> topic_ids = master_client.QueryForTopics();
    for (auto topic_id : topic_ids)
      cout << "Found topic: " << topic_id << endl;
  } else if (SARGS_GET_STRING("--mode") == "add") {
    TopicConfig topic_config;
    topic_config.port = SARGS_GET_UINT16("--port");
    topic_config.name = SARGS_GET_STRING("--name");
    master_client.AddTopic(topic_config);
  }

  return 0;
}