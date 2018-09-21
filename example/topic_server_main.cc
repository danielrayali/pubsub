#include <pubsub.h>
#include <sargs.h>

int main(int argc, char* argv[]) {
  SARGS_REQUIRED_FLAG_VALUE("--name", "-n", "The name of the topic");
  SARGS_REQUIRED_FLAG_VALUE("--port", "-p", "The local port the server should run on");
  SARGS_INITIALIZE(argc, argv);

  pubsub::TopicConfig topic_config;
  topic_config.name = SARGS_GET_STRING("--name");
  topic_config.port = SARGS_GET_UINT16("--port");

  pubsub::TopicServer topic_server(topic_config);
  topic_server.Run();
  pubsub::WaitForSignal();
  topic_server.Stop();
  return 0;
}
