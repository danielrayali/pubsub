#include <pubsub.h>
#include <sargs.h>
#include <iostream>

#include "spdlog/spdlog.h"

using namespace std;

void callback(pubsub::Buffer&& buffer) {
  string received(reinterpret_cast<char*>(buffer.Get()), buffer.Size() - 1);
  spdlog::info("Data received: {}", received);
}

int main(int argc, char* argv[]) {
  SARGS_REQUIRED_FLAG_VALUE("--topic-port", "-tp", "The topic server's port to connect to");
  SARGS_INITIALIZE(argc, argv);

  uint16_t port = SARGS_GET_UINT16("-tp");
  pubsub::SubscriberClient subscriber_client("localhost", port);
  subscriber_client.Register(callback);
  pubsub::WaitForSignal();
  subscriber_client.Deregister();
  return 0;
}
