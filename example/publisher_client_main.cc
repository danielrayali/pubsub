#include <pubsub.h>

int main(int argc, char* argv[]) {
  pubsub::PublisherClient publisher_client("localhost", 10001);
  std::string data = "Hello, World!";
  publisher_client.Publish(data.c_str(), data.size() + 1);
  return 0;
}
