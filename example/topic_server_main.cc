#include <pubsub.h>

int main(int argc, char* argv[]) {
  pubsub::TopicServer topic_server;
  topic_server.Run();
  pubsub::WaitForSignal();
  topic_server.Stop();  
  return 0;
}
